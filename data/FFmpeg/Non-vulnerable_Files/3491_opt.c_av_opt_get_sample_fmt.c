int av_opt_get_sample_fmt(void *obj, const char *name, int search_flags, enum AVSampleFormat *out_fmt)
{
    return get_format(obj, name, search_flags, out_fmt, AV_OPT_TYPE_SAMPLE_FMT, "sample");
}
