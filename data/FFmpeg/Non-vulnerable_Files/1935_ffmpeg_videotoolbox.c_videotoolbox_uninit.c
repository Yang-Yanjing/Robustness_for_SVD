static void videotoolbox_uninit(AVCodecContext *s)
{
    InputStream *ist = s->opaque;
    VTContext  *vt = ist->hwaccel_ctx;
    ist->hwaccel_uninit        = NULL;
    ist->hwaccel_retrieve_data = NULL;
    av_frame_free(&vt->tmp_frame);
    if (ist->hwaccel_id == HWACCEL_VIDEOTOOLBOX) {
#if CONFIG_VIDEOTOOLBOX
        av_videotoolbox_default_free(s);
#endif
    } else {
#if CONFIG_VDA
        av_vda_default_free(s);
#endif
    }
    av_freep(&ist->hwaccel_ctx);
}
