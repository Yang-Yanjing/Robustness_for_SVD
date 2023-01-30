static void sbr_reset(AACContext *ac, SpectralBandReplication *sbr)
{
    int err;
    err = sbr_make_f_master(ac, sbr, &sbr->spectrum_params);
    if (err >= 0)
        err = sbr_make_f_derived(ac, sbr);
    if (err < 0) {
        av_log(ac->avctx, AV_LOG_ERROR,
               "SBR reset failed. Switching SBR to pure upsampling mode.\n");
        sbr_turnoff(sbr);
    }
}
