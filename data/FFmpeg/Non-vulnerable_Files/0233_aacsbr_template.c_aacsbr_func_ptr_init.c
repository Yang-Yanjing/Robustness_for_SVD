static void aacsbr_func_ptr_init(AACSBRContext *c)
{
    c->sbr_lf_gen            = sbr_lf_gen;
    c->sbr_hf_assemble       = sbr_hf_assemble;
    c->sbr_x_gen             = sbr_x_gen;
    c->sbr_hf_inverse_filter = sbr_hf_inverse_filter;
#if !USE_FIXED
    if(ARCH_MIPS)
        ff_aacsbr_func_ptr_init_mips(c);
#endif
}
