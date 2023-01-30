/** Create VDPAU video mixer */
static VdpVideoMixer MixerCreate(filter_t *filter, bool import)
{
    filter_sys_t *sys = filter->p_sys;
    VdpVideoMixer mixer;
    VdpStatus err;
    VdpBool ok;
    /* Check for potentially useful features */
    VdpVideoMixerFeature featv[5];
    unsigned featc = 0;
    int algo = var_InheritInteger(filter, "vdpau-deinterlace");
    bool ivtc = false;
    if (algo == VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL_SPATIAL)
    {
        err = vdp_video_mixer_query_feature_support(sys->vdp, sys->device,
                                                    algo, &ok);
        if (err == VDP_STATUS_OK && ok == VDP_TRUE)
            msg_Dbg(filter, "using video mixer %s feature",
                    "temporal-spatial deinterlace");
        else
            algo = VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL; /* fallback */
    }
    if (algo == VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL)
    {
        err = vdp_video_mixer_query_feature_support(sys->vdp, sys->device,
                                                    algo, &ok);
        if (err == VDP_STATUS_OK && ok == VDP_TRUE)
            msg_Dbg(filter, "using video mixer %s feature",
                    "temporal deinterlace");
        else
            algo = -1;
    }
    if (algo >= 0)
    {
        featv[featc++] = algo;
        ivtc = var_InheritBool(filter, "vdpau-ivtc");
        if (ivtc)
        {
            err = vdp_video_mixer_query_feature_support(sys->vdp, sys->device,
                                VDP_VIDEO_MIXER_FEATURE_INVERSE_TELECINE, &ok);
            if (err == VDP_STATUS_OK && ok == VDP_TRUE)
                msg_Dbg(filter, "using video mixer %s feature",
                        "inverse telecine");
            featv[featc++] = VDP_VIDEO_MIXER_FEATURE_INVERSE_TELECINE;
        }
    }
    const float noise = var_InheritFloat(filter, "vdpau-noise-reduction");
    if (noise > 0.f)
    {
        err = vdp_video_mixer_query_feature_support(sys->vdp, sys->device,
                                 VDP_VIDEO_MIXER_FEATURE_NOISE_REDUCTION, &ok);
        if (err == VDP_STATUS_OK && ok == VDP_TRUE)
        {
            msg_Dbg(filter, "using video mixer %s feature", "noise reduction");
            featv[featc++] = VDP_VIDEO_MIXER_FEATURE_NOISE_REDUCTION;
        }
    }
    err = vdp_video_mixer_query_feature_support(sys->vdp, sys->device,
                                       VDP_VIDEO_MIXER_FEATURE_SHARPNESS, &ok);
    if (err == VDP_STATUS_OK && ok == VDP_TRUE)
    {
        msg_Dbg(filter, "using video mixer %s feature", "sharpness");
        featv[featc++] = VDP_VIDEO_MIXER_FEATURE_SHARPNESS;
    }
    const int offset = VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L1 - 1;
    unsigned level = var_InheritInteger(filter, "vdpau-scaling");
    while (level > 0)
    {
        err = vdp_video_mixer_query_feature_support(sys->vdp, sys->device,
                                                    offset + level, &ok);
        if (err == VDP_STATUS_OK && ok == VDP_TRUE)
        {
            msg_Dbg(filter, "using video mixer high quality scaling L%u",
                    level);
            featv[featc++] = offset + level;
            break;
        }
        level--; /* fallback to lower quality */
    }
    /* Create the mixer */
    VdpVideoMixerParameter parms[3] = {
        VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_WIDTH,
        VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_HEIGHT,
        VDP_VIDEO_MIXER_PARAMETER_CHROMA_TYPE,
    };
    uint32_t width = filter->fmt_in.video.i_width;
    uint32_t height = import ? filter->fmt_in.video.i_visible_height
                             : filter->fmt_in.video.i_height;
    const void *values[3] = { &width, &height, &sys->chroma, };
    err = vdp_video_mixer_create(sys->vdp, sys->device, featc, featv,
                                 3, parms, values, &mixer);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(filter, "video %s %s failure: %s", "mixer", "creation",
                vdp_get_error_string(sys->vdp, err));
        return VDP_INVALID_HANDLE;
    }
    msg_Dbg(filter, "using video mixer %"PRIu32, mixer);
    /* Set initial features and attributes */
    VdpVideoMixerAttribute attrv[3];
    const void *valv[3];
    unsigned attrc = 0;
    VdpCSCMatrix csc;
    uint8_t chroma_skip;
    featc = 0;
    if (MixerSetupColors(filter, NULL, &csc) == VDP_STATUS_OK)
    {
        attrv[attrc] = VDP_VIDEO_MIXER_ATTRIBUTE_CSC_MATRIX;
        valv[attrc] = &csc;
        attrc++;
    }
    if (algo >= 0)
    {
        featv[featc++] = algo;
        if (ivtc)
            featv[featc++] = VDP_VIDEO_MIXER_FEATURE_INVERSE_TELECINE;
        chroma_skip = var_InheritBool(filter, "vdpau-chroma-skip");
        attrv[attrc] = VDP_VIDEO_MIXER_ATTRIBUTE_SKIP_CHROMA_DEINTERLACE;
        valv[attrc] = &chroma_skip;
        attrc++;
    }
    if (noise > 0.f)
    {
        featv[featc++] = VDP_VIDEO_MIXER_FEATURE_NOISE_REDUCTION;
        attrv[attrc] = VDP_VIDEO_MIXER_ATTRIBUTE_NOISE_REDUCTION_LEVEL;
        valv[attrc] = &noise;
        attrc++;
    }
    if (level > 0)
        featv[featc++] = offset + level;
    if (featc > 0)
    {
        VdpBool enablev[featc];
        for (unsigned i = 0; i < featc; i++)
            enablev[i] = VDP_TRUE;
        err = vdp_video_mixer_set_feature_enables(sys->vdp, mixer,
                                                  featc, featv, enablev);
        if (err != VDP_STATUS_OK)
            msg_Err(filter, "video %s %s failure: %s", "mixer", "features",
                    vdp_get_error_string(sys->vdp, err));
    }
    if (attrc > 0)
    {
        err = vdp_video_mixer_set_attribute_values(sys->vdp, mixer,
                                                   attrc, attrv, valv);
        if (err != VDP_STATUS_OK)
            msg_Err(filter, "video %s %s failure: %s", "mixer", "attributes",
                    vdp_get_error_string(sys->vdp, err));
    }
    return mixer;
}
