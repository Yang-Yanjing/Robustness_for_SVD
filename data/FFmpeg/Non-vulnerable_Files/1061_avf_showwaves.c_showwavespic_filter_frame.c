static int showwavespic_filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ShowWavesContext *showwaves = ctx->priv;
    int16_t *p = (int16_t *)insamples->data[0];
    int ret = 0;
    if (showwaves->single_pic) {
        struct frame_node *f;
        ret = alloc_out_frame(showwaves, p, inlink, outlink, insamples);
        if (ret < 0)
            goto end;
        
        f = av_malloc(sizeof(*f));
        if (!f) {
            ret = AVERROR(ENOMEM);
            goto end;
        }
        f->frame = insamples;
        f->next = NULL;
        if (!showwaves->last_frame) {
            showwaves->audio_frames =
            showwaves->last_frame   = f;
        } else {
            showwaves->last_frame->next = f;
            showwaves->last_frame = f;
        }
        showwaves->total_samples += insamples->nb_samples;
        return 0;
    }
end:
    av_frame_free(&insamples);
    return ret;
}
