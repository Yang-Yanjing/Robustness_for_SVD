static int showwaves_filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ShowWavesContext *showwaves = ctx->priv;
    const int nb_samples = insamples->nb_samples;
    AVFrame *outpicref = showwaves->outpicref;
    int16_t *p = (int16_t *)insamples->data[0];
    int nb_channels = inlink->channels;
    int i, j, ret = 0;
    const int n = showwaves->n;
    const int x = 255 / ((showwaves->split_channels ? 1 : nb_channels) * n); 
    const int ch_height = showwaves->split_channels ? outlink->h / nb_channels : outlink->h;
    
    for (i = 0; i < nb_samples; i++) {
        ret = alloc_out_frame(showwaves, p, inlink, outlink, insamples);
        if (ret < 0)
            goto end;
        outpicref = showwaves->outpicref;
        for (j = 0; j < nb_channels; j++) {
            uint8_t *buf = outpicref->data[0] + showwaves->buf_idx;
            const int linesize = outpicref->linesize[0];
            if (showwaves->split_channels)
                buf += j*ch_height*linesize;
            showwaves->draw_sample(buf, ch_height, linesize, *p++,
                                   &showwaves->buf_idy[j], x);
        }
        showwaves->sample_count_mod++;
        if (showwaves->sample_count_mod == n) {
            showwaves->sample_count_mod = 0;
            showwaves->buf_idx++;
        }
        if (showwaves->buf_idx == showwaves->w)
            if ((ret = push_frame(outlink)) < 0)
                break;
        outpicref = showwaves->outpicref;
    }
end:
    av_frame_free(&insamples);
    return ret;
}
