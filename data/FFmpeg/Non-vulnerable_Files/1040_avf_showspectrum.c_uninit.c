static av_cold void uninit(AVFilterContext *ctx)
{
    ShowSpectrumContext *s = ctx->priv;
    int i;
    av_freep(&s->combine_buffer);
    av_rdft_end(s->rdft);
    for (i = 0; i < s->nb_display_channels; i++)
        av_freep(&s->rdft_data[i]);
    av_freep(&s->rdft_data);
    av_freep(&s->window_func_lut);
    av_frame_free(&s->outpicref);
}
