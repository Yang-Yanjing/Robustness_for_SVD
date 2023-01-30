}
static block_t * avi_HeaderCreateidx1( sout_mux_t *p_mux )
{
    sout_mux_sys_t      *p_sys = p_mux->p_sys;
    uint32_t            i_idx1_size;
    buffer_out_t        bo;
    i_idx1_size = 16 * p_sys->idx1.i_entry_count + 8;
    bo_Init( &bo, i_idx1_size );
    memset( bo.p_block->p_buffer, 0, i_idx1_size);
    bo_AddFCC( &bo, "idx1" );
    bo_AddDWordLE( &bo, i_idx1_size - 8);
    for( unsigned i = 0; i < p_sys->idx1.i_entry_count; i++ )
    {
        bo_AddFCC( &bo, p_sys->idx1.entry[i].fcc );
        bo_AddDWordLE( &bo, p_sys->idx1.entry[i].i_flags );
        bo_AddDWordLE( &bo, p_sys->idx1.entry[i].i_pos );
        bo_AddDWordLE( &bo, p_sys->idx1.entry[i].i_length );
    }
    return( bo.p_block );
}
