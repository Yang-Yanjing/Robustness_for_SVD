int av_opt_copy(void *dst, FF_CONST_AVUTIL55 void *src)
{
    const AVOption *o = NULL;
    const AVClass *c;
    int ret = 0;
    if (!src)
        return 0;
    c = *(AVClass**)src;
    if (*(AVClass**)dst && c != *(AVClass**)dst)
        return AVERROR(EINVAL);
    while ((o = av_opt_next(src, o))) {
        void *field_dst = ((uint8_t*)dst) + o->offset;
        void *field_src = ((uint8_t*)src) + o->offset;
        uint8_t **field_dst8 = (uint8_t**)field_dst;
        uint8_t **field_src8 = (uint8_t**)field_src;
        if (o->type == AV_OPT_TYPE_STRING) {
            if (*field_dst8 != *field_src8)
                av_freep(field_dst8);
            *field_dst8 = av_strdup(*field_src8);
            if (*field_src8 && !*field_dst8)
                ret = AVERROR(ENOMEM);
        } else if (o->type == AV_OPT_TYPE_BINARY) {
            int len = *(int*)(field_src8 + 1);
            if (*field_dst8 != *field_src8)
                av_freep(field_dst8);
            *field_dst8 = av_memdup(*field_src8, len);
            if (len && !*field_dst8) {
                ret = AVERROR(ENOMEM);
                len = 0;
            }
            *(int*)(field_dst8 + 1) = len;
        } else if (o->type == AV_OPT_TYPE_CONST) {
            
        } else if (o->type == AV_OPT_TYPE_DICT) {
            AVDictionary **sdict = (AVDictionary **) field_src;
            AVDictionary **ddict = (AVDictionary **) field_dst;
            if (*sdict != *ddict)
                av_dict_free(ddict);
            *ddict = NULL;
            av_dict_copy(ddict, *sdict, 0);
            if (av_dict_count(*sdict) != av_dict_count(*ddict))
                ret = AVERROR(ENOMEM);
        } else {
            memcpy(field_dst, field_src, opt_size(o->type));
        }
    }
    return ret;
}
