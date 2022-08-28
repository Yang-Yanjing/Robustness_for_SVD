static int parse_device_name(AVFormatContext *avctx)
{
    struct dshow_ctx *ctx = avctx->priv_data;
    char **device_name = ctx->device_name;
    char *name = av_strdup(avctx->filename);
    char *tmp = name;
    int ret = 1;
    char *type;
    while ((type = strtok(tmp, "="))) {
        char *token = strtok(NULL, ":");
        tmp = NULL;
        if        (!strcmp(type, "video")) {
            device_name[0] = token;
        } else if (!strcmp(type, "audio")) {
            device_name[1] = token;
        } else {
            device_name[0] = NULL;
            device_name[1] = NULL;
            break;
        }
    }
    if (!device_name[0] && !device_name[1]) {
        ret = 0;
    } else {
        if (device_name[0])
            device_name[0] = av_strdup(device_name[0]);
        if (device_name[1])
            device_name[1] = av_strdup(device_name[1]);
    }
    av_free(name);
    return ret;
}
