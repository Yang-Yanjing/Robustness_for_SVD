void ff_acelp_vectors_init_mips(ACELPVContext *c)
{
#if HAVE_INLINE_ASM
    c->weighted_vector_sumf = ff_weighted_vector_sumf_mips;
#endif
}
