static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    ShowSpectrumContext *s = ctx->priv;
    int i, rdft_bits, win_size, h;
    outlink->w = s->w;
    outlink->h = s->h;
    h = (s->mode == COMBINED) ? outlink->h : outlink->h / inlink->channels;
    s->channel_height = h;
    
    for (rdft_bits = 1; 1 << rdft_bits < 2 * h; rdft_bits++);
    win_size = 1 << rdft_bits;
    
    if (rdft_bits != s->rdft_bits) {
        AVFrame *outpicref;
        av_rdft_end(s->rdft);
        s->rdft = av_rdft_init(rdft_bits, DFT_R2C);
        if (!s->rdft) {
            av_log(ctx, AV_LOG_ERROR, "Unable to create RDFT context. "
                   "The window size might be too high.\n");
            return AVERROR(EINVAL);
        }
        s->rdft_bits = rdft_bits;
        


        for (i = 0; i < s->nb_display_channels; i++)
            av_freep(&s->rdft_data[i]);
        av_freep(&s->rdft_data);
        s->nb_display_channels = inlink->channels;
        s->rdft_data = av_calloc(s->nb_display_channels, sizeof(*s->rdft_data));
        if (!s->rdft_data)
            return AVERROR(ENOMEM);
        for (i = 0; i < s->nb_display_channels; i++) {
            s->rdft_data[i] = av_calloc(win_size, sizeof(**s->rdft_data));
            if (!s->rdft_data[i])
                return AVERROR(ENOMEM);
        }
        
        s->window_func_lut =
            av_realloc_f(s->window_func_lut, win_size,
                         sizeof(*s->window_func_lut));
        if (!s->window_func_lut)
            return AVERROR(ENOMEM);
        switch (s->win_func) {
        case WFUNC_NONE:
            for (i = 0; i < win_size; i++)
                s->window_func_lut[i] = 1.;
            break;
        case WFUNC_HANN:
            for (i = 0; i < win_size; i++)
                s->window_func_lut[i] = .5f * (1 - cos(2*M_PI*i / (win_size-1)));
            break;
        case WFUNC_HAMMING:
            for (i = 0; i < win_size; i++)
                s->window_func_lut[i] = .54f - .46f * cos(2*M_PI*i / (win_size-1));
            break;
        case WFUNC_BLACKMAN: {
            for (i = 0; i < win_size; i++)
                s->window_func_lut[i] = .42f - .5f*cos(2*M_PI*i / (win_size-1)) + .08f*cos(4*M_PI*i / (win_size-1));
            break;
        }
        default:
            av_assert0(0);
        }
        
        av_frame_free(&s->outpicref);
        s->outpicref = outpicref =
            ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!outpicref)
            return AVERROR(ENOMEM);
        outlink->sample_aspect_ratio = (AVRational){1,1};
        for (i = 0; i < outlink->h; i++) {
            memset(outpicref->data[0] + i * outpicref->linesize[0],   0, outlink->w);
            memset(outpicref->data[1] + i * outpicref->linesize[1], 128, outlink->w);
            memset(outpicref->data[2] + i * outpicref->linesize[2], 128, outlink->w);
        }
    }
    if (s->xpos >= outlink->w)
        s->xpos = 0;
    outlink->frame_rate = av_make_q(inlink->sample_rate, win_size);
    if (s->sliding == FULLFRAME)
        outlink->frame_rate.den *= outlink->w;
    inlink->min_samples = inlink->max_samples = inlink->partial_buf_size =
        win_size;
    s->combine_buffer =
        av_realloc_f(s->combine_buffer, outlink->h * 3,
                     sizeof(*s->combine_buffer));
    av_log(ctx, AV_LOG_VERBOSE, "s:%dx%d RDFT window size:%d\n",
           s->w, s->h, win_size);
    return 0;
}
