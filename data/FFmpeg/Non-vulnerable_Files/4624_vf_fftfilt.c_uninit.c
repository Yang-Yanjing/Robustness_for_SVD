static av_cold void uninit(AVFilterContext *ctx)
{
    FFTFILTContext *fftfilt = ctx->priv;
    int i;
    for (i = 0; i < MAX_PLANES; i++) {
        av_free(fftfilt->rdft_hdata[i]);
        av_free(fftfilt->rdft_vdata[i]);
        av_expr_free(fftfilt->weight_expr[i]);
        av_free(fftfilt->weight[i]);
    }
}
