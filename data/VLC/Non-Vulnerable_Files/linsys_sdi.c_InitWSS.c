}
static int InitWSS( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    vbi_raw_decoder_init( &p_sys->rd_wss );
    p_sys->rd_wss.scanning        = 625;
    p_sys->rd_wss.sampling_format = VBI_PIXFMT_UYVY;
    p_sys->rd_wss.sampling_rate   = 13.5e6;
    p_sys->rd_wss.bytes_per_line  = 720 * 2;
    p_sys->rd_wss.offset          = 9.5e-6 * 13.5e6;
    p_sys->rd_wss.start[0] = 23;
    p_sys->rd_wss.count[0] = 1;
    p_sys->rd_wss.start[1] = 0;
    p_sys->rd_wss.count[1] = 0;
    p_sys->rd_wss.interlaced = FALSE;
    p_sys->rd_wss.synchronous = TRUE;
    if ( vbi_raw_decoder_add_services( &p_sys->rd_wss,
                                       VBI_SLICED_WSS_625,
                                       /* strict */ 2 ) == 0 )
    {
        msg_Warn( p_demux, "cannot initialize zvbi for WSS" );
        vbi_raw_decoder_destroy ( &p_sys->rd_telx );
        return VLC_EGENERIC;
    }
    p_sys->p_wss_buffer = malloc( p_sys->i_width * 2 );
    if( !p_sys->p_wss_buffer )
    {
        vbi_raw_decoder_destroy ( &p_sys->rd_telx );
        return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}
