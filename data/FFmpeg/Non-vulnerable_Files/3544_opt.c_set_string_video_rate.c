static int set_string_video_rate(void *obj, const AVOption *o, const char *val, AVRational *dst)
{
    int ret;
    if (!val) {
        ret = AVERROR(EINVAL);
    } else {
        ret = av_parse_video_rate(dst, val);
    }
    if (ret < 0)
        av_log(obj, AV_LOG_ERROR, "Unable to parse option value \"%s\" as video rate\n", val);
    return ret;
}
