const char *avutil_license(void)
{
#define LICENSE_PREFIX "libavutil license: "
    return LICENSE_PREFIX FFMPEG_LICENSE + sizeof(LICENSE_PREFIX) - 1;
}
