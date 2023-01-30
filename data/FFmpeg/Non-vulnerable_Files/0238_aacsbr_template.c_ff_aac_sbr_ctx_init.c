av_cold void AAC_RENAME(ff_aac_sbr_ctx_init)(AACContext *ac, SpectralBandReplication *sbr)
{
    if(sbr->mdct.mdct_bits)
        return;
    sbr->kx[0] = sbr->kx[1];
    sbr_turnoff(sbr);
    sbr->data[0].synthesis_filterbank_samples_offset = SBR_SYNTHESIS_BUF_SIZE - (1280 - 128);
    sbr->data[1].synthesis_filterbank_samples_offset = SBR_SYNTHESIS_BUF_SIZE - (1280 - 128);
    


    AAC_RENAME_32(ff_mdct_init)(&sbr->mdct,     7, 1, 1.0 / (64 * 32768.0));
    AAC_RENAME_32(ff_mdct_init)(&sbr->mdct_ana, 7, 1, -2.0 * 32768.0);
    AAC_RENAME(ff_ps_ctx_init)(&sbr->ps);
    AAC_RENAME(ff_sbrdsp_init)(&sbr->dsp);
    aacsbr_func_ptr_init(&sbr->c);
}
