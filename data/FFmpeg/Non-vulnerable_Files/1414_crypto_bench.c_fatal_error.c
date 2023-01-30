static const uint8_t *hardcoded_key = "FFmpeg is the best program ever.";
static void fatal_error(const char *tag)
{
    av_log(NULL, AV_LOG_ERROR, "Fatal error: %s\n", tag);
    exit(1);
}
