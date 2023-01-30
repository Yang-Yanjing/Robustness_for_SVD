}
static void dirac_AddBlockEncap( block_t **pp_block, dirac_block_encap_t *p_dbe )
{
    /* must not fail, fixby: adding a p_priv to block_t */
    fake_block_t *p_fake = xcalloc( 1, sizeof( *p_fake ) );
    block_t *in = *pp_block, *out = &p_fake->fake;
    block_Init( out, in->p_buffer, in->i_buffer );
    out->i_flags = in->i_flags;
    out->i_nb_samples = in->i_nb_samples;
    out->i_pts = in->i_pts;
    out->i_dts = in->i_dts;
    out->i_length = in->i_length;
    out->pf_release = dirac_ReleaseBlockAndEncap;
    p_fake->p_orig = in;
    p_fake->p_dbe = p_dbe;
    *pp_block = out;
}
