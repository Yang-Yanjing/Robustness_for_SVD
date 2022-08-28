void av_opt_free(void *obj)
{
    const AVOption *o = NULL;
    while ((o = av_opt_next(obj, o))) {
        switch (o->type) {
        case AV_OPT_TYPE_STRING:
        case AV_OPT_TYPE_BINARY:
            av_freep((uint8_t *)obj + o->offset);
            break;
        case AV_OPT_TYPE_DICT:
            av_dict_free((AVDictionary **)(((uint8_t *)obj) + o->offset));
            break;
        default:
            break;
        }
    }
}
