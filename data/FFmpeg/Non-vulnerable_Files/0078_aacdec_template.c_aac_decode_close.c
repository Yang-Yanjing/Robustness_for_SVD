static av_cold int aac_decode_close(AVCodecContext *avctx)
{
    AACContext *ac = avctx->priv_data;
    int i, type;
    for (i = 0; i < MAX_ELEM_ID; i++) {
        for (type = 0; type < 4; type++) {
            if (ac->che[type][i])
                AAC_RENAME(ff_aac_sbr_ctx_close)(&ac->che[type][i]->sbr);
            av_freep(&ac->che[type][i]);
        }
    }
    ff_mdct_end(&ac->mdct);
    ff_mdct_end(&ac->mdct_small);
    ff_mdct_end(&ac->mdct_ld);
    ff_mdct_end(&ac->mdct_ltp);
#if !USE_FIXED
    ff_imdct15_uninit(&ac->mdct480);
#endif
    av_freep(&ac->fdsp);
    return 0;
}
