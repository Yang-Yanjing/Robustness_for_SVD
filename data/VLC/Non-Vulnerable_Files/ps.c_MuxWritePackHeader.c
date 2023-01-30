}
static void MuxWritePackHeader( sout_mux_t *p_mux, block_t **p_buf,
                                mtime_t i_dts )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    bits_buffer_t bits;
    block_t *p_hdr;
    mtime_t i_scr;
    int i_mux_rate;
    i_scr = (i_dts - p_sys->i_dts_delay) * 9 / 100;
    p_hdr = block_Alloc( 18 );
    p_hdr->i_pts = p_hdr->i_dts = i_dts;
    bits_initwrite( &bits, 14, p_hdr->p_buffer );
    bits_write( &bits, 32, 0x01ba );
    /* The spec specifies that the mux rate must be rounded upwards */
    i_mux_rate = (p_sys->i_instant_bitrate + 8 * 50 - 1 ) / (8 * 50);
    if( p_sys->b_mpeg2 )
    {
        bits_write( &bits, 2, 0x01 );
    }
    else
    {
        bits_write( &bits, 4, 0x02 );
    }
    bits_write( &bits, 3, ( i_scr >> 30 )&0x07 );
    bits_write( &bits, 1,  1 ); // marker
    bits_write( &bits, 15, ( i_scr >> 15 )&0x7fff );
    bits_write( &bits, 1,  1 ); // marker
    bits_write( &bits, 15, i_scr&0x7fff );
    bits_write( &bits, 1,  1 ); // marker
    if( p_sys->b_mpeg2 )
    {
        bits_write( &bits, 9,  0 ); // src extension
    }
    bits_write( &bits, 1,  1 );     // marker
    bits_write( &bits, 22, i_mux_rate);
    bits_write( &bits, 1,  1 );     // marker
    if( p_sys->b_mpeg2 )
    {
        bits_write( &bits, 1,  1 );     // marker
        bits_write( &bits, 5,  0x1f );  // reserved
        bits_write( &bits, 3,  0 );     // stuffing bytes
    }
    p_hdr->i_buffer = p_sys->b_mpeg2 ? 14: 12;
    block_ChainAppend( p_buf, p_hdr );
}
