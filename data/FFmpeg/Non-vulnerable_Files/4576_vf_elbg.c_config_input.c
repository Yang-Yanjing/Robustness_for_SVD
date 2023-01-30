static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ELBGContext *elbg = ctx->priv;
    elbg->pix_desc = av_pix_fmt_desc_get(inlink->format);
    elbg->codeword_length = inlink->w * inlink->h;
    elbg->codeword = av_realloc_f(elbg->codeword, elbg->codeword_length,
                                  NB_COMPONENTS * sizeof(*elbg->codeword));
    if (!elbg->codeword)
        return AVERROR(ENOMEM);
    elbg->codeword_closest_codebook_idxs =
        av_realloc_f(elbg->codeword_closest_codebook_idxs, elbg->codeword_length,
                     sizeof(*elbg->codeword_closest_codebook_idxs));
    if (!elbg->codeword_closest_codebook_idxs)
        return AVERROR(ENOMEM);
    elbg->codebook = av_realloc_f(elbg->codebook, elbg->codebook_length,
                                  NB_COMPONENTS * sizeof(*elbg->codebook));
    if (!elbg->codebook)
        return AVERROR(ENOMEM);
    ff_fill_rgba_map(elbg->rgba_map, inlink->format);
    return 0;
}
