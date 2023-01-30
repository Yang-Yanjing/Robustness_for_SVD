AVFILTER_DEFINE_CLASS(showwaves);
static av_cold void uninit(AVFilterContext *ctx)
{
    ShowWavesContext *showwaves = ctx->priv;
    av_frame_free(&showwaves->outpicref);
    av_freep(&showwaves->buf_idy);
    if (showwaves->single_pic) {
        struct frame_node *node = showwaves->audio_frames;
        while (node) {
            struct frame_node *tmp = node;
            node = node->next;
            av_frame_free(&tmp->frame);
            av_freep(&tmp);
        }
        av_freep(&showwaves->sum);
        showwaves->last_frame = NULL;
    }
}
