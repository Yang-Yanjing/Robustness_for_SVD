}
static void Close(vlc_object_t *this)
{
    encoder_t *enc = (encoder_t *)this;
    encoder_sys_t *sys = enc->p_sys;
    MFXVideoENCODE_Close(sys->session);
    MFXClose(sys->session);
    /* if (enc->fmt_out.p_extra) */
    /*     free(enc->fmt_out.p_extra); */
    if (sys->frames.size)
        qsv_frame_pool_Destroy(&sys->frames);
    if (sys->tasks)
        free(sys->tasks);
    free(sys);
}
