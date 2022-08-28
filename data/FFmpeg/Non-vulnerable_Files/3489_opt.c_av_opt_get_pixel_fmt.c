int av_opt_get_pixel_fmt(void *obj, const char *name, int search_flags, enum AVPixelFormat *out_fmt)
{
    return get_format(obj, name, search_flags, out_fmt, AV_OPT_TYPE_PIXEL_FMT, "pixel");
}
