}
static void BlockMetaCopy( block_t *restrict out, const block_t *in )
{
    out->p_next    = in->p_next;
    out->i_nb_samples = in->i_nb_samples;
    out->i_dts     = in->i_dts;
    out->i_pts     = in->i_pts;
    out->i_flags   = in->i_flags;
    out->i_length  = in->i_length;
}
