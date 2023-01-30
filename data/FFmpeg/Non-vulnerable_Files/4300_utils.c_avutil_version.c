unsigned avutil_version(void)
{
    static int checks_done;
    if (checks_done)
        return LIBAVUTIL_VERSION_INT;
#if FF_API_VDPAU
    av_assert0(AV_PIX_FMT_VDA_VLD == 81); 
#endif
    av_assert0(AV_SAMPLE_FMT_DBLP == 9);
    av_assert0(AVMEDIA_TYPE_ATTACHMENT == 4);
    av_assert0(AV_PICTURE_TYPE_BI == 7);
    av_assert0(LIBAVUTIL_VERSION_MICRO >= 100);
    av_assert0(HAVE_MMX2 == HAVE_MMXEXT);
    av_assert0(((size_t)-1) > 0); 
    if (av_sat_dadd32(1, 2) != 5) {
        av_log(NULL, AV_LOG_FATAL, "Libavutil has been build with a broken binutils, please upgrade binutils and rebuild\n");
        abort();
    }
    if (llrint(1LL<<60) != 1LL<<60) {
        av_log(NULL, AV_LOG_ERROR, "Libavutil has been linked to a broken llrint()\n");
    }
    checks_done = 1;
    return LIBAVUTIL_VERSION_INT;
}
