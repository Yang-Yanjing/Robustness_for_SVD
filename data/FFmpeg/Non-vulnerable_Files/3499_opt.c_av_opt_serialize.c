int av_opt_serialize(void *obj, int opt_flags, int flags, char **buffer,
                     const char key_val_sep, const char pairs_sep)
{
    const AVOption *o = NULL;
    uint8_t *buf;
    AVBPrint bprint;
    int ret, cnt = 0;
    const char special_chars[] = {pairs_sep, key_val_sep, '\0'};
    if (pairs_sep == '\0' || key_val_sep == '\0' || pairs_sep == key_val_sep ||
        pairs_sep == '\\' || key_val_sep == '\\') {
        av_log(obj, AV_LOG_ERROR, "Invalid separator(s) found.");
        return AVERROR(EINVAL);
    }
    if (!obj || !buffer)
        return AVERROR(EINVAL);
    *buffer = NULL;
    av_bprint_init(&bprint, 64, AV_BPRINT_SIZE_UNLIMITED);
    while (o = av_opt_next(obj, o)) {
        if (o->type == AV_OPT_TYPE_CONST)
            continue;
        if ((flags & AV_OPT_SERIALIZE_OPT_FLAGS_EXACT) && o->flags != opt_flags)
            continue;
        else if (((o->flags & opt_flags) != opt_flags))
            continue;
        if (flags & AV_OPT_SERIALIZE_SKIP_DEFAULTS && av_opt_is_set_to_default(obj, o) > 0)
            continue;
        if ((ret = av_opt_get(obj, o->name, 0, &buf)) < 0) {
            av_bprint_finalize(&bprint, NULL);
            return ret;
        }
        if (buf) {
            if (cnt++)
                av_bprint_append_data(&bprint, &pairs_sep, 1);
            av_bprint_escape(&bprint, o->name, special_chars, AV_ESCAPE_MODE_BACKSLASH, 0);
            av_bprint_append_data(&bprint, &key_val_sep, 1);
            av_bprint_escape(&bprint, buf, special_chars, AV_ESCAPE_MODE_BACKSLASH, 0);
            av_freep(&buf);
        }
    }
    av_bprint_finalize(&bprint, buffer);
    return 0;
}
