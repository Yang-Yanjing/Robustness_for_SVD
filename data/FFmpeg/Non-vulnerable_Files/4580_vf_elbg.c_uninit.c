static av_cold void uninit(AVFilterContext *ctx)
{
    ELBGContext *elbg = ctx->priv;
    av_freep(&elbg->codebook);
    av_freep(&elbg->codeword);
    av_freep(&elbg->codeword_closest_codebook_idxs);
}
