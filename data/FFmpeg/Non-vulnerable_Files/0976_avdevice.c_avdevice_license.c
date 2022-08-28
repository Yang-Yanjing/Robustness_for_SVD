const char * avdevice_license(void)
{
#define LICENSE_PREFIX "libavdevice license: "
    return LICENSE_PREFIX FFMPEG_LICENSE + sizeof(LICENSE_PREFIX) - 1;
}
