}
static int InitTelx( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    vbi_raw_decoder_init( &p_sys->rd_telx );
    p_sys->rd_telx.scanning        = 625;
    p_sys->rd_telx.sampling_format = VBI_PIXFMT_UYVY;
    p_sys->rd_telx.sampling_rate   = 13.5e6;
    p_sys->rd_telx.bytes_per_line  = 720 * 2;
    p_sys->rd_telx.offset          = 9.5e-6 * 13.5e6;
    p_sys->rd_telx.start[0] = p_sys->i_telx_line + 1;
    p_sys->rd_telx.count[0] = p_sys->i_telx_count;
    p_sys->rd_telx.start[1] = p_sys->i_telx_line + 1 + 313;
    p_sys->rd_telx.count[1] = p_sys->i_telx_count;
    p_sys->rd_telx.interlaced = FALSE;
    p_sys->rd_telx.synchronous = TRUE;
    if ( vbi_raw_decoder_add_services( &p_sys->rd_telx, VBI_SLICED_TELETEXT_B,
                                       0 ) == 0 )
    {
        msg_Warn( p_demux, "cannot initialize zvbi for Teletext" );
        vbi_raw_decoder_destroy ( &p_sys->rd_telx );
        return VLC_EGENERIC;
    }
    p_sys->p_telx_buffer = malloc( p_sys->i_telx_count * p_sys->i_width * 4 );
    if( !p_sys->p_telx_buffer )
    {
        vbi_raw_decoder_destroy ( &p_sys->rd_telx );
        return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}
