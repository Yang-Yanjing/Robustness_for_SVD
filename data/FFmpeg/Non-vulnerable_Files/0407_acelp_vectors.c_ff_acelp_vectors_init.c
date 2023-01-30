void ff_acelp_vectors_init(ACELPVContext *c)
{
    c->weighted_vector_sumf   = ff_weighted_vector_sumf;
    if(HAVE_MIPSFPU)
        ff_acelp_vectors_init_mips(c);
}
