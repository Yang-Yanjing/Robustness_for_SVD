av_cold void AAC_RENAME(ff_aac_sbr_ctx_close)(SpectralBandReplication *sbr)
{
    AAC_RENAME_32(ff_mdct_end)(&sbr->mdct);
    AAC_RENAME_32(ff_mdct_end)(&sbr->mdct_ana);
}
