AVFILTER_DEFINE_CLASS(showcqt);
static av_cold void uninit(AVFilterContext *ctx)
{
    int k;
    ShowCQTContext *s = ctx->priv;
    av_fft_end(s->fft_context);
    s->fft_context = NULL;
    for (k = 0; k < VIDEO_WIDTH; k++)
        av_freep(&s->coeffs[k]);
    av_freep(&s->fft_data);
    av_freep(&s->fft_result_left);
    av_freep(&s->fft_result_right);
    av_freep(&s->coeff_sort);
    av_freep(&s->spectogram);
    av_freep(&s->font_alpha);
    av_frame_free(&s->outpicref);
}
