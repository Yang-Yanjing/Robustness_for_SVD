static av_cold int init(AVFilterContext *ctx)
{
    FliteContext *flite = ctx->priv;
    int ret = 0;
    if (flite->list_voices) {
        list_voices(ctx, "\n");
        return AVERROR_EXIT;
    }
    if (!flite_inited) {
        if (flite_init() < 0) {
            av_log(ctx, AV_LOG_ERROR, "flite initialization failed\n");
            return AVERROR_UNKNOWN;
        }
        flite_inited++;
    }
    if ((ret = select_voice(&flite->voice_entry, flite->voice_str, ctx)) < 0)
        return ret;
    flite->voice = flite->voice_entry->voice;
    if (flite->textfile && flite->text) {
        av_log(ctx, AV_LOG_ERROR,
               "Both text and textfile options set: only one must be specified\n");
        return AVERROR(EINVAL);
    }
    if (flite->textfile) {
        uint8_t *textbuf;
        size_t textbuf_size;
        if ((ret = av_file_map(flite->textfile, &textbuf, &textbuf_size, 0, ctx)) < 0) {
            av_log(ctx, AV_LOG_ERROR,
                   "The text file '%s' could not be read: %s\n",
                   flite->textfile, av_err2str(ret));
            return ret;
        }
        if (!(flite->text = av_malloc(textbuf_size+1)))
            return AVERROR(ENOMEM);
        memcpy(flite->text, textbuf, textbuf_size);
        flite->text[textbuf_size] = 0;
        av_file_unmap(textbuf, textbuf_size);
    }
    if (!flite->text) {
        av_log(ctx, AV_LOG_ERROR,
               "No speech text specified, specify the 'text' or 'textfile' option\n");
        return AVERROR(EINVAL);
    }
    
    flite->wave = flite_text_to_wave(flite->text, flite->voice);
    flite->wave_samples    = flite->wave->samples;
    flite->wave_nb_samples = flite->wave->num_samples;
    return 0;
}
