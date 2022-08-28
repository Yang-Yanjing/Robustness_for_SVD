unsigned avfilter_version(void)
{
    av_assert0(LIBAVFILTER_VERSION_MICRO >= 100);
    return LIBAVFILTER_VERSION_INT;
}
