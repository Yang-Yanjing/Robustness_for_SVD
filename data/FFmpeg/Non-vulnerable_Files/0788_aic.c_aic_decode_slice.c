static int aic_decode_slice(AICContext *ctx, int mb_x, int mb_y,
                            const uint8_t *src, int src_size)
{
    GetBitContext gb;
    int ret, i, mb, blk;
    int slice_width = FFMIN(ctx->slice_width, ctx->mb_width - mb_x);
    uint8_t *Y, *C[2];
    uint8_t *dst;
    int16_t *base_y = ctx->data_ptr[COEFF_LUMA];
    int16_t *base_c = ctx->data_ptr[COEFF_CHROMA];
    int16_t *ext_y  = ctx->data_ptr[COEFF_LUMA_EXT];
    int16_t *ext_c  = ctx->data_ptr[COEFF_CHROMA_EXT];
    const int ystride = ctx->frame->linesize[0];
    Y = ctx->frame->data[0] + mb_x * 16 + mb_y * 16 * ystride;
    for (i = 0; i < 2; i++)
        C[i] = ctx->frame->data[i + 1] + mb_x * 8
               + mb_y * 8 * ctx->frame->linesize[i + 1];
    init_get_bits(&gb, src, src_size * 8);
    memset(ctx->slice_data, 0,
           sizeof(*ctx->slice_data) * slice_width * AIC_BAND_COEFFS);
    for (i = 0; i < NUM_BANDS; i++)
        if ((ret = aic_decode_coeffs(&gb, ctx->data_ptr[i],
                                     i, slice_width,
                                     !ctx->interlaced)) < 0)
            return ret;
    for (mb = 0; mb < slice_width; mb++) {
        for (blk = 0; blk < 4; blk++) {
            if (!ctx->interlaced)
                recombine_block(ctx->block, ctx->scantable.permutated,
                                &base_y, &ext_y);
            else
                recombine_block_il(ctx->block, ctx->scantable.permutated,
                                   &base_y, &ext_y, blk);
            unquant_block(ctx->block, ctx->quant, ctx->quant_matrix);
            ctx->idsp.idct(ctx->block);
            if (!ctx->interlaced) {
                dst = Y + (blk >> 1) * 8 * ystride + (blk & 1) * 8;
                ctx->idsp.put_signed_pixels_clamped(ctx->block, dst, ystride);
            } else {
                dst = Y + (blk & 1) * 8 + (blk >> 1) * ystride;
                ctx->idsp.put_signed_pixels_clamped(ctx->block, dst,
                                                    ystride * 2);
            }
        }
        Y += 16;
        for (blk = 0; blk < 2; blk++) {
            recombine_block(ctx->block, ctx->scantable.permutated,
                            &base_c, &ext_c);
            unquant_block(ctx->block, ctx->quant, ctx->quant_matrix);
            ctx->idsp.idct(ctx->block);
            ctx->idsp.put_signed_pixels_clamped(ctx->block, C[blk],
                                                ctx->frame->linesize[blk + 1]);
            C[blk] += 8;
        }
    }
    return 0;
}
