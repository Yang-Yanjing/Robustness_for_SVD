const char postproc_ffversion[] = "FFmpeg version " FFMPEG_VERSION;
unsigned postproc_version(void)
{
    av_assert0(LIBPOSTPROC_VERSION_MICRO >= 100);
    return LIBPOSTPROC_VERSION_INT;
}
