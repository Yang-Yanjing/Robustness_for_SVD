static void aacdec_init(AACContext *c)
{
    c->imdct_and_windowing                      = imdct_and_windowing;
    c->apply_ltp                                = apply_ltp;
    c->apply_tns                                = apply_tns;
    c->windowing_and_mdct_ltp                   = windowing_and_mdct_ltp;
    c->update_ltp                               = update_ltp;
#if USE_FIXED
    c->vector_pow43                             = vector_pow43;
    c->subband_scale                            = subband_scale;
#endif
#if !USE_FIXED
    if(ARCH_MIPS)
        ff_aacdec_init_mips(c);
#endif 
}
