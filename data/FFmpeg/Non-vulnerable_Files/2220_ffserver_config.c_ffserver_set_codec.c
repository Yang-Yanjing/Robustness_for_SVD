static int ffserver_set_codec(AVCodecContext *ctx, const char *codec_name,
                              FFServerConfig *config)
{
    int ret;
    AVCodec *codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec || codec->type != ctx->codec_type) {
        report_config_error(config->filename, config->line_num, AV_LOG_ERROR,
                            &config->errors,
                            "Invalid codec name: '%s'\n", codec_name);
        return 0;
    }
    if (ctx->codec_id == AV_CODEC_ID_NONE && !ctx->priv_data) {
        if ((ret = avcodec_get_context_defaults3(ctx, codec)) < 0)
            return ret;
        ctx->codec = codec;
    }
    if (ctx->codec_id != codec->id)
        report_config_error(config->filename, config->line_num, AV_LOG_ERROR,
                            &config->errors,
                            "Inconsistent configuration: trying to set '%s' "
                            "codec option, but '%s' codec is used previously\n",
                            codec_name, avcodec_get_name(ctx->codec_id));
    return 0;
}
