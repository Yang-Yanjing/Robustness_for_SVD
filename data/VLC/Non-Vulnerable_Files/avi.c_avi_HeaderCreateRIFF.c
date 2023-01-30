}
static block_t *avi_HeaderCreateRIFF( sout_mux_t *p_mux )
{
    sout_mux_sys_t      *p_sys = p_mux->p_sys;
    int                 i_stream;
    int                 i_junk;
    buffer_out_t        bo;
    struct
    {
        int i_riffsize;
        int i_hdrllistsize;
        int i_hdrldatastart;
    } offsets;
    bo_Init( &bo, HDR_BASE_SIZE );
    bo_AddFCC( &bo, "RIFF" );
    offsets.i_riffsize = bo.p_block->i_buffer;
    bo_AddDWordLE( &bo, 0xEFBEADDE );
    bo_AddFCC( &bo, "AVI " );
    bo_AddFCC( &bo, "LIST" );
    /* HDRL List size should exclude following data in HDR buffer
     *  -12 (RIFF, RIFF size, 'AVI ' tag),
     *  - 8 (hdr1 LIST tag and its size)
     *  - 12 (movi LIST tag, size, 'movi' listType )
     */
    offsets.i_hdrllistsize = bo.p_block->i_buffer;
    bo_AddDWordLE( &bo, 0xEFBEADDE );
    bo_AddFCC( &bo, "hdrl" );
    offsets.i_hdrldatastart = bo.p_block->i_buffer;
    avi_HeaderAdd_avih( p_mux, &bo );
    for( i_stream = 0; i_stream < p_sys->i_streams; i_stream++ )
    {
        avi_HeaderAdd_strl( &bo, &p_sys->stream[i_stream] );
    }
    /* align on 16 bytes */
    int i_align = ( ( bo.p_block->i_buffer + 12 + 0xE ) & ~ 0xF );
    i_junk = i_align - bo.p_block->i_buffer;
    bo_AddFCC( &bo, "JUNK" );
    bo_AddDWordLE( &bo, i_junk );
    for( int i=0; i< i_junk; i++ )
    {
        bo_AddByte( &bo, 0 );
    }
    /* Now set hdrl size */
    bo_SetDWordLE( &bo, offsets.i_hdrllistsize,
                   bo.p_block->i_buffer - offsets.i_hdrldatastart );
    avi_HeaderAdd_INFO( p_mux, &bo );
    bo_AddFCC( &bo, "LIST" );
    bo_AddDWordLE( &bo, p_sys->i_movi_size + 4 );
    bo_AddFCC( &bo, "movi" );
    /* Now set RIFF size */
    bo_SetDWordLE( &bo, offsets.i_riffsize, bo.p_block->i_buffer - 8
                   + p_sys->i_movi_size + p_sys->i_idx1_size );
    return( bo.p_block );
}
