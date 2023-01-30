#endif
void block_Init( block_t *restrict b, void *buf, size_t size )
{
    /* Fill all fields to their default */
    b->p_next = NULL;
    b->p_buffer = buf;
    b->i_buffer = size;
    b->p_start = buf;
    b->i_size = size;
    b->i_flags = 0;
    b->i_nb_samples = 0;
    b->i_pts =
    b->i_dts = VLC_TS_INVALID;
    b->i_length = 0;
#ifndef NDEBUG
    b->pf_release = BlockNoRelease;
#endif
}
