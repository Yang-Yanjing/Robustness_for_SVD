static int set_string_sample_fmt(void *obj, const AVOption *o, const char *val, uint8_t *dst)
{
    return set_string_fmt(obj, o, val, dst,
                          AV_SAMPLE_FMT_NB, av_get_sample_fmt, "sample format");
}
