static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    FSPPContext *fspp = ctx->priv;
    const int h = FFALIGN(inlink->h + 16, 16);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    fspp->hsub = desc->log2_chroma_w;
    fspp->vsub = desc->log2_chroma_h;
    fspp->temp_stride = FFALIGN(inlink->w + 16, 16);
    fspp->temp = av_malloc_array(fspp->temp_stride, h * sizeof(*fspp->temp));
    fspp->src  = av_malloc_array(fspp->temp_stride, h * sizeof(*fspp->src));
    if (!fspp->temp || !fspp->src)
        return AVERROR(ENOMEM);
    if (!fspp->use_bframe_qp && !fspp->qp) {
        fspp->non_b_qp_alloc_size = FF_CEIL_RSHIFT(inlink->w, 4) * FF_CEIL_RSHIFT(inlink->h, 4);
        fspp->non_b_qp_table = av_calloc(fspp->non_b_qp_alloc_size, sizeof(*fspp->non_b_qp_table));
        if (!fspp->non_b_qp_table)
            return AVERROR(ENOMEM);
    }
    fspp->store_slice  = store_slice_c;
    fspp->store_slice2 = store_slice2_c;
    fspp->mul_thrmat   = mul_thrmat_c;
    fspp->column_fidct = column_fidct_c;
    fspp->row_idct     = row_idct_c;
    fspp->row_fdct     = row_fdct_c;
    if (ARCH_X86)
        ff_fspp_init_x86(fspp);
    return 0;
}
