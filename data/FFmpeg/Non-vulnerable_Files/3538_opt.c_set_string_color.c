static int set_string_color(void *obj, const AVOption *o, const char *val, uint8_t *dst)
{
    int ret;
    if (!val) {
        return 0;
    } else {
        ret = av_parse_color(dst, val, -1, obj);
        if (ret < 0)
            av_log(obj, AV_LOG_ERROR, "Unable to parse option value \"%s\" as color\n", val);
        return ret;
    }
    return 0;
}
