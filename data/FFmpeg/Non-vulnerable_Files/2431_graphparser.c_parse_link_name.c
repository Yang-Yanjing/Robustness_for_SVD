static char *parse_link_name(const char **buf, void *log_ctx)
{
    const char *start = *buf;
    char *name;
    (*buf)++;
    name = av_get_token(buf, "]");
    if (!name)
        goto fail;
    if (!name[0]) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Bad (empty?) label found in the following: \"%s\".\n", start);
        goto fail;
    }
    if (*(*buf)++ != ']') {
        av_log(log_ctx, AV_LOG_ERROR,
               "Mismatched '[' found in the following: \"%s\".\n", start);
    fail:
        av_freep(&name);
    }
    return name;
}
