static av_cold void lame_window_init(AacPsyContext *ctx, AVCodecContext *avctx)
{
    int i, j;
    for (i = 0; i < avctx->channels; i++) {
        AacPsyChannel *pch = &ctx->ch[i];
        if (avctx->flags & AV_CODEC_FLAG_QSCALE)
            pch->attack_threshold = psy_vbr_map[avctx->global_quality / FF_QP2LAMBDA].st_lrm;
        else
            pch->attack_threshold = lame_calc_attack_threshold(avctx->bit_rate / avctx->channels / 1000);
        for (j = 0; j < AAC_NUM_BLOCKS_SHORT * PSY_LAME_NUM_SUBBLOCKS; j++)
            pch->prev_energy_subshort[j] = 10.0f;
    }
}
