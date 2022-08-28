static av_cold void uninit(AVFilterContext *ctx)
{
    USPPContext *uspp = ctx->priv;
    int i;
    for (i = 0; i < 3; i++) {
        av_freep(&uspp->temp[i]);
        av_freep(&uspp->src[i]);
    }
    for (i = 0; i < (1 << uspp->log2_count); i++) {
        avcodec_close(uspp->avctx_enc[i]);
        av_freep(&uspp->avctx_enc[i]);
    }
    av_freep(&uspp->non_b_qp_table);
    av_freep(&uspp->outbuf);
    av_frame_free(&uspp->frame);
}
