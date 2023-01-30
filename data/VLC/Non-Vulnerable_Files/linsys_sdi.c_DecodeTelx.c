}
static int DecodeTelx( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    vbi_sliced p_sliced[p_sys->i_telx_count * 2];
    int i_nb_slices = vbi_raw_decode( &p_sys->rd_telx, p_sys->p_telx_buffer,
                                      p_sliced );
    if ( i_nb_slices )
    {
        /* 3, 7, 11, 15, etc. */
        int i_nb_slices_rounded = 3 + (i_nb_slices / 4) * 4;
        int i;
        uint8_t *p;
        block_t *p_block = block_Alloc( 1 + i_nb_slices_rounded * 46 );
        if( unlikely( !p_block ) )
            return VLC_ENOMEM;
        p_block->p_buffer[0] = 0x10; /* FIXME ? data_identifier */
        p = p_block->p_buffer + 1;
        for ( i = 0; i < i_nb_slices; i++ )
        {
            int i_line = p_sliced[i].line;
            p[0] = 0x3; /* FIXME data_unit_id == subtitles */
            p[1] = 0x2c; /* data_unit_length */
            /* reserved | field_parity (kind of inverted) | line */
            p[2] = 0xc0 | (i_line > 313 ? 0 : 0x20) | (i_line % 313);
            p[3] = 0xe4; /* framing code */
            for ( int j = 0; j < 42; j++ )
                p[4 + j] = vbi_rev8( p_sliced[i].data[j] );
            p += 46;
        }
        /* Let's stuff */
        for ( ; i < i_nb_slices_rounded; i++ )
        {
            p[0] = 0xff;
            p[1] = 0x2c;
            memset( p + 2, 0xff, 44 );
            p += 46;
        }
        p_block->i_dts = p_block->i_pts = p_sys->i_next_date;
        es_out_Send( p_demux->out, p_sys->p_es_telx, p_block );
    }
    return VLC_SUCCESS;
}
