}
static picture_t *Render(filter_t *filter, picture_t *src, bool import)
{
    filter_sys_t *sys = filter->p_sys;
    VdpStatus err;
    if (unlikely(src->context == NULL))
    {
        msg_Err(filter, "corrupt VDPAU video surface");
        picture_Release(src);
        return NULL;
    }
    picture_t *dst = OutputAllocate(filter);
    /* Corner case: different VDPAU instances decoding and rendering */
    vlc_vdp_video_field_t *field = src->context;
    if (field->frame->vdp != sys->vdp)
    {
        video_format_t fmt = src->format;
        switch (sys->chroma)
        {
             case VDP_CHROMA_TYPE_420: fmt.i_chroma = VLC_CODEC_NV12; break;
             case VDP_CHROMA_TYPE_422: fmt.i_chroma = VLC_CODEC_UYVY; break;
             case VDP_CHROMA_TYPE_444: fmt.i_chroma = VLC_CODEC_NV24; break;
             default: assert(0);
        }
        picture_t *pic = picture_NewFromFormat(&fmt);
        if (likely(pic != NULL))
        {
            pic = VideoExport(filter, src, pic);
            if (pic != NULL)
                src = VideoImport(filter, pic);
            else
                src = NULL;
        }
        else
        {
            picture_Release(src);
            src = NULL;
        }
    }
    /* Update history and take "present" picture field */
    if (likely(src != NULL))
    {
        sys->history[MAX_PAST + MAX_FUTURE].field =
                                              vlc_vdp_video_copy(src->context);
        sys->history[MAX_PAST + MAX_FUTURE].date = src->date;
        sys->history[MAX_PAST + MAX_FUTURE].force = src->b_force;
        picture_Release(src);
    }
    else
    {
        sys->history[MAX_PAST + MAX_FUTURE].field = NULL;
        sys->history[MAX_PAST + MAX_FUTURE].force = false;
    }
    if (dst == NULL)
        goto skip;
    vlc_vdp_video_field_t *f = sys->history[MAX_PAST].field;
    if (f == NULL)
    {   /* There is no present field, probably just starting playback. */
        if (!sys->history[MAX_PAST + MAX_FUTURE].force)
            goto error;
        /* If the picture is forced, ignore deinterlacing and fast forward. */
        /* FIXME: Remove the forced hack pictures in video output core and
         * allow the last field of a video to be rendered properly. */
        while (sys->history[MAX_PAST].field == NULL)
        {
            f = sys->history[0].field;
            if (f != NULL)
                f->destroy(f);
            memmove(sys->history, sys->history + 1,
                    sizeof (sys->history[0]) * (MAX_PAST + MAX_FUTURE));
            sys->history[MAX_PAST + MAX_FUTURE].field = NULL;
        }
        f = sys->history[MAX_PAST].field;
    }
    dst->date = sys->history[MAX_PAST].date;
    dst->b_force = sys->history[MAX_PAST].force;
    /* Enable/Disable features */
    const VdpVideoMixerFeature features[] = {
        VDP_VIDEO_MIXER_FEATURE_SHARPNESS,
    };
    const VdpBool enables[] = {
        f->sharpen != 0.f,
    };
    err = vdp_video_mixer_set_feature_enables(sys->vdp, sys->mixer,
                  sizeof (features) / sizeof (features[0]), features, enables);
    if (err != VDP_STATUS_OK)
        msg_Err(filter, "video %s %s failure: %s", "mixer", "features",
                vdp_get_error_string(sys->vdp, err));
    /* Configure mixer depending on upstream video filters */
    VdpVideoMixerAttribute attrs[2] = {
        VDP_VIDEO_MIXER_ATTRIBUTE_SHARPNESS_LEVEL,
    };
    const void *values[2] = {
        &f->sharpen,
    };
    unsigned count = 1;
    VdpCSCMatrix csc;
    if ((sys->procamp.brightness != f->procamp.brightness
      || sys->procamp.contrast != f->procamp.contrast
      || sys->procamp.saturation != f->procamp.saturation
      || sys->procamp.hue != f->procamp.hue)
     && (MixerSetupColors(filter, &f->procamp, &csc) == VDP_STATUS_OK))
    {
        attrs[count] = VDP_VIDEO_MIXER_ATTRIBUTE_CSC_MATRIX;
        values[count] = &csc;
        count++;
    }
    err = vdp_video_mixer_set_attribute_values(sys->vdp, sys->mixer,
                                               count, attrs, values);
    if (err != VDP_STATUS_OK)
        msg_Err(filter, "video %s %s failure: %s", "mixer", "attributes",
                vdp_get_error_string(sys->vdp, err));
    /* Check video orientation, allocate intermediate surface if needed */
    bool swap = ORIENT_IS_SWAP(filter->fmt_in.video.orientation);
    bool hflip = false, vflip = false;
    switch (filter->fmt_in.video.orientation)
    {
        case ORIENT_TOP_LEFT:
        case ORIENT_RIGHT_TOP:
            break;
        case ORIENT_TOP_RIGHT:
        case ORIENT_RIGHT_BOTTOM:
            hflip = true;
            break;
        case ORIENT_BOTTOM_LEFT:
        case ORIENT_LEFT_TOP:
            vflip = true;
            break;
        case ORIENT_BOTTOM_RIGHT:
        case ORIENT_LEFT_BOTTOM:
            vflip = hflip = true;
            break;
    }
    VdpOutputSurface output = dst->p_sys->surface;
    if (swap)
    {
        VdpRGBAFormat fmt;
        uint32_t width, height;
        err = vdp_output_surface_get_parameters(sys->vdp, output,
                                                &fmt, &width, &height);
        if (err != VDP_STATUS_OK)
        {
            msg_Err(filter, "output %s %s failure: %s", "surface", "query",
                    vdp_get_error_string(sys->vdp, err));
            goto error;
        }
        err = vdp_output_surface_create(sys->vdp, sys->device,
                                        fmt, height, width, &output);
        if (err != VDP_STATUS_OK)
        {
            msg_Err(filter, "output %s %s failure: %s", "surface", "creation",
                    vdp_get_error_string(sys->vdp, err));
            goto error;
        }
    }
    /* Render video into output */
    VdpVideoMixerPictureStructure structure = f->structure;
    VdpVideoSurface past[MAX_PAST];
    VdpVideoSurface surface = f->frame->surface;
    VdpVideoSurface future[MAX_FUTURE];
    VdpRect src_rect = {
        filter->fmt_in.video.i_x_offset, filter->fmt_in.video.i_y_offset,
        filter->fmt_in.video.i_x_offset, filter->fmt_in.video.i_y_offset,
    };
    if (import)
        src_rect.y0 = src_rect.y1 = 0;
    if (hflip)
        src_rect.x0 += filter->fmt_in.video.i_visible_width;
    else
        src_rect.x1 += filter->fmt_in.video.i_visible_width;
    if (vflip)
        src_rect.y0 += filter->fmt_in.video.i_visible_height;
    else
        src_rect.y1 += filter->fmt_in.video.i_visible_height;
    VdpRect dst_rect = {
        0, 0,
        swap ? filter->fmt_out.video.i_visible_height
             : filter->fmt_out.video.i_visible_width,
        swap ? filter->fmt_out.video.i_visible_width
             : filter->fmt_out.video.i_visible_height,
    };
    for (unsigned i = 0; i < MAX_PAST; i++)
    {
        f = sys->history[(MAX_PAST - 1) - i].field;
        past[i] = (f != NULL) ? f->frame->surface : VDP_INVALID_HANDLE;
    }
    for (unsigned i = 0; i < MAX_FUTURE; i++)
    {
        f = sys->history[(MAX_PAST + 1) + i].field;
        future[i] = (f != NULL) ? f->frame->surface : VDP_INVALID_HANDLE;
    }
    err = vdp_video_mixer_render(sys->vdp, sys->mixer, VDP_INVALID_HANDLE,
                                 NULL, structure,
                                 MAX_PAST, past, surface, MAX_FUTURE, future,
                                 &src_rect, output, &dst_rect, &dst_rect, 0,
                                 NULL);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(filter, "video %s %s failure: %s", "mixer", "rendering",
                vdp_get_error_string(sys->vdp, err));
        goto error;
    }
    if (swap)
    {
        err = vdp_output_surface_render_output_surface(sys->vdp,
            dst->p_sys->surface, NULL, output, NULL, NULL, NULL,
            VDP_OUTPUT_SURFACE_RENDER_ROTATE_90);
        vdp_output_surface_destroy(sys->vdp, output);
        if (err != VDP_STATUS_OK)
        {
            msg_Err(filter, "output %s %s failure: %s", "surface", "render",
                    vdp_get_error_string(sys->vdp, err));
            goto error;
        }
    }
skip:
    f = sys->history[0].field;
    if (f != NULL)
        f->destroy(f); /* Release oldest field */
    memmove(sys->history, sys->history + 1, /* Advance history */
            sizeof (sys->history[0]) * (MAX_PAST + MAX_FUTURE));
    return dst;
error:
    picture_Release(dst);
    dst = NULL;
    goto skip;
}
