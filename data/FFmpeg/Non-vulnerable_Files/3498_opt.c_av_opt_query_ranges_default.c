int av_opt_query_ranges_default(AVOptionRanges **ranges_arg, void *obj, const char *key, int flags)
{
    AVOptionRanges *ranges = av_mallocz(sizeof(*ranges));
    AVOptionRange **range_array = av_mallocz(sizeof(void*));
    AVOptionRange *range = av_mallocz(sizeof(*range));
    const AVOption *field = av_opt_find(obj, key, NULL, 0, flags);
    int ret;
    *ranges_arg = NULL;
    if (!ranges || !range || !range_array || !field) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    ranges->range = range_array;
    ranges->range[0] = range;
    ranges->nb_ranges = 1;
    ranges->nb_components = 1;
    range->is_range = 1;
    range->value_min = field->min;
    range->value_max = field->max;
    switch (field->type) {
    case AV_OPT_TYPE_INT:
    case AV_OPT_TYPE_INT64:
    case AV_OPT_TYPE_PIXEL_FMT:
    case AV_OPT_TYPE_SAMPLE_FMT:
    case AV_OPT_TYPE_FLOAT:
    case AV_OPT_TYPE_DOUBLE:
    case AV_OPT_TYPE_DURATION:
    case AV_OPT_TYPE_COLOR:
    case AV_OPT_TYPE_CHANNEL_LAYOUT:
        break;
    case AV_OPT_TYPE_STRING:
        range->component_min = 0;
        range->component_max = 0x10FFFF; 
        range->value_min = -1;
        range->value_max = INT_MAX;
        break;
    case AV_OPT_TYPE_RATIONAL:
        range->component_min = INT_MIN;
        range->component_max = INT_MAX;
        break;
    case AV_OPT_TYPE_IMAGE_SIZE:
        range->component_min = 0;
        range->component_max = INT_MAX/128/8;
        range->value_min = 0;
        range->value_max = INT_MAX/8;
        break;
    case AV_OPT_TYPE_VIDEO_RATE:
        range->component_min = 1;
        range->component_max = INT_MAX;
        range->value_min = 1;
        range->value_max = INT_MAX;
        break;
    default:
        ret = AVERROR(ENOSYS);
        goto fail;
    }
    *ranges_arg = ranges;
    return 1;
fail:
    av_free(ranges);
    av_free(range);
    av_free(range_array);
    return ret;
}
