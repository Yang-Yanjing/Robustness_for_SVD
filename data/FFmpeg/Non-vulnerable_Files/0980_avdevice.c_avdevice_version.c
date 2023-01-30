unsigned avdevice_version(void)
{
    av_assert0(LIBAVDEVICE_VERSION_MICRO >= 100);
    return LIBAVDEVICE_VERSION_INT;
}
