static int set_string_pixel_fmt(void *obj, const AVOption *o, const char *val, uint8_t *dst)
{
    return set_string_fmt(obj, o, val, dst,
                          AV_PIX_FMT_NB, av_get_pix_fmt, "pixel format");
}
