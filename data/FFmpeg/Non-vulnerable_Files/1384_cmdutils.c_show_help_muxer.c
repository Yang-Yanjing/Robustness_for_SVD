static void show_help_muxer(const char *name)
{
    const AVCodecDescriptor *desc;
    const AVOutputFormat *fmt = av_guess_format(name, NULL, NULL);
    if (!fmt) {
        av_log(NULL, AV_LOG_ERROR, "Unknown format '%s'.\n", name);
        return;
    }
    printf("Muxer %s [%s]:\n", fmt->name, fmt->long_name);
    if (fmt->extensions)
        printf("    Common extensions: %s.\n", fmt->extensions);
    if (fmt->mime_type)
        printf("    Mime type: %s.\n", fmt->mime_type);
    if (fmt->video_codec != AV_CODEC_ID_NONE &&
        (desc = avcodec_descriptor_get(fmt->video_codec))) {
        printf("    Default video codec: %s.\n", desc->name);
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE &&
        (desc = avcodec_descriptor_get(fmt->audio_codec))) {
        printf("    Default audio codec: %s.\n", desc->name);
    }
    if (fmt->subtitle_codec != AV_CODEC_ID_NONE &&
        (desc = avcodec_descriptor_get(fmt->subtitle_codec))) {
        printf("    Default subtitle codec: %s.\n", desc->name);
    }
    if (fmt->priv_class)
        show_help_children(fmt->priv_class, AV_OPT_FLAG_ENCODING_PARAM);
}
