static int alloc_out_frame(ShowWavesContext *showwaves, const int16_t *p,
                           const AVFilterLink *inlink, AVFilterLink *outlink,
                           const AVFrame *in)
{
    if (!showwaves->outpicref) {
        int j;
        AVFrame *out = showwaves->outpicref =
            ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        out->width  = outlink->w;
        out->height = outlink->h;
        out->pts = in->pts + av_rescale_q((p - (int16_t *)in->data[0]) / inlink->channels,
                                          av_make_q(1, inlink->sample_rate),
                                          outlink->time_base);
        for (j = 0; j < outlink->h; j++)
            memset(out->data[0] + j*out->linesize[0], 0, outlink->w);
    }
    return 0;
}
