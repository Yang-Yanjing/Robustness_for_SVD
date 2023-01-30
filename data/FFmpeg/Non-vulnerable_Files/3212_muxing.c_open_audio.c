static void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;
    c = ost->st->codec;
    
    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
        exit(1);
    }
    
    ost->t     = 0;
    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    
    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;
    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;
    ost->frame     = alloc_audio_frame(c->sample_fmt, c->channel_layout,
                                       c->sample_rate, nb_samples);
    ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
                                       c->sample_rate, nb_samples);
    
        ost->swr_ctx = swr_alloc();
        if (!ost->swr_ctx) {
            fprintf(stderr, "Could not allocate resampler context\n");
            exit(1);
        }
        
        av_opt_set_int       (ost->swr_ctx, "in_channel_count",   c->channels,       0);
        av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     c->sample_rate,    0);
        av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
        av_opt_set_int       (ost->swr_ctx, "out_channel_count",  c->channels,       0);
        av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
        av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);
        
        if ((ret = swr_init(ost->swr_ctx)) < 0) {
            fprintf(stderr, "Failed to initialize the resampling context\n");
            exit(1);
        }
}
