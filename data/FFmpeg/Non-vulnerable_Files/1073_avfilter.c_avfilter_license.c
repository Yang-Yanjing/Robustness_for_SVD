const char *avfilter_license(void)
{
#define LICENSE_PREFIX "libavfilter license: "
    return LICENSE_PREFIX FFMPEG_LICENSE + sizeof(LICENSE_PREFIX) - 1;
}
