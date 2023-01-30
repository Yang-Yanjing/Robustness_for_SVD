};
static void DisplaySubpicture(vout_display_t *vd, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    struct md5_s hash;
    InitMD5(&hash);
    if (subpicture) {
        for (subpicture_region_t *r = subpicture->p_region; r != NULL; r = r->p_next) {
            AddMD5(&hash, &r->i_x, sizeof(r->i_x));
            AddMD5(&hash, &r->i_y, sizeof(r->i_y));
            AddMD5(&hash, &r->fmt.i_visible_width, sizeof(r->fmt.i_visible_width));
            AddMD5(&hash, &r->fmt.i_visible_height, sizeof(r->fmt.i_visible_height));
            AddMD5(&hash, &r->fmt.i_x_offset, sizeof(r->fmt.i_x_offset));
            AddMD5(&hash, &r->fmt.i_y_offset, sizeof(r->fmt.i_y_offset));
            const int pixels_offset = r->fmt.i_y_offset * r->p_picture->p->i_pitch +
                                      r->fmt.i_x_offset * r->p_picture->p->i_pixel_pitch;
            for (int y = 0; y < r->fmt.i_visible_height; y++)
                AddMD5(&hash, &r->p_picture->p->p_pixels[pixels_offset + y*r->p_picture->p->i_pitch], r->fmt.i_visible_width);
        }
    }
    EndMD5(&hash);
    if (!memcmp(hash.buf, sys->hash, 16))
        return;
    memcpy(sys->hash, hash.buf, 16);
    jobject jsurf = jni_LockAndGetSubtitlesSurface();
    if (sys->window && jsurf != sys->jsurf)
    {
        sys->native_window.winRelease(sys->window);
        sys->window = NULL;
    }
    sys->jsurf = jsurf;
    if (!sys->window)
    {
        JNIEnv *p_env;
        jni_attach_thread(&p_env, THREAD_NAME);
        sys->window = sys->native_window.winFromSurface(p_env, jsurf);
        jni_detach_thread();
    }
    ANativeWindow_Buffer buf = { 0 };
    int32_t err = sys->native_window.winLock(sys->window, &buf, NULL);
    if (err) {
        jni_UnlockAndroidSurface();
        return;
    }
    if (buf.width >= sys->fmt.i_width && buf.height >= sys->fmt.i_height)
    {
        /* Wrap the NativeWindow corresponding to the subtitles surface in a picture_t */
        picture_t *picture = sys->subtitles_picture;
        picture->p[0].p_pixels = (uint8_t*)buf.bits;
        picture->p[0].i_lines = buf.height;
        picture->p[0].i_pitch = picture->p[0].i_pixel_pitch * buf.stride;
        /* Clear the subtitles surface. */
        memset(picture->p[0].p_pixels, 0, picture->p[0].i_pitch * picture->p[0].i_lines);
        if (subpicture)
        {
            /* Allocate a blending filter if needed. */
            if (unlikely(!sys->p_spu_blend))
                sys->p_spu_blend = filter_NewBlend(VLC_OBJECT(vd), &picture->format);
            picture_BlendSubpicture(picture, sys->p_spu_blend, subpicture);
        }
    }
    sys->native_window.unlockAndPost(sys->window);
    jni_UnlockAndroidSurface();
}
