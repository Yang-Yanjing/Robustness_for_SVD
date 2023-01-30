int av_opt_get_image_size(void *obj, const char *name, int search_flags, int *w_out, int *h_out)
{
    void *dst, *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != AV_OPT_TYPE_IMAGE_SIZE) {
        av_log(obj, AV_LOG_ERROR,
               "The value for option '%s' is not an image size.\n", name);
        return AVERROR(EINVAL);
    }
    dst = ((uint8_t*)target_obj) + o->offset;
    if (w_out) *w_out = *(int *)dst;
    if (h_out) *h_out = *((int *)dst+1);
    return 0;
}
