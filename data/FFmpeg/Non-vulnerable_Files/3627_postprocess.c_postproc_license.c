const char *postproc_license(void)
{
#define LICENSE_PREFIX "libpostproc license: "
    return LICENSE_PREFIX FFMPEG_LICENSE + sizeof(LICENSE_PREFIX) - 1;
}
