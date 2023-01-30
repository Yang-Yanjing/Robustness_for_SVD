}
block_t *scan_GetM3U( scan_t *p_scan )
{
    vlc_object_t *p_obj = p_scan->p_obj;
    block_t *p_playlist = NULL;
    if( p_scan->i_service <= 0 )
        return NULL;
    /* */
    qsort( p_scan->pp_service, p_scan->i_service, sizeof(scan_service_t*), ScanServiceCmp );
    /* */
    p_playlist = BlockString( "#EXTM3U\n\n" );/* */
    for( int i = 0; i < p_scan->i_service; i++ )
    {
        scan_service_t *s = p_scan->pp_service[i];
        if( s->type == SERVICE_UNKNOWN )
        {
            /* We should only select service that have been described by SDT */
            msg_Dbg( p_obj, "scan_GetM3U: ignoring service number %d", s->i_program );
            continue;
        }
        const char *psz_type;
        switch( s->type )
        {
        case SERVICE_DIGITAL_TELEVISION:       psz_type = "Digital television"; break;
        case SERVICE_DIGITAL_TELEVISION_AC_SD: psz_type = "Digital television advanced codec SD"; break;
        case SERVICE_DIGITAL_TELEVISION_AC_HD: psz_type = "Digital television advanced codec HD"; break;
        case SERVICE_DIGITAL_RADIO:            psz_type = "Digital radio"; break;
        default:
            psz_type = "Unknown";
            break;
        }
        msg_Warn( p_obj, "scan_GetM3U: service number %d type '%s' name '%s' channel %d cypted=%d| network_id %d (nit:%d sdt:%d)| f=%d bw=%d snr=%d modulation=%d",
                  s->i_program, psz_type, s->psz_name, s->i_channel, s->b_crypted,
                  s->i_network_id, s->i_nit_version, s->i_sdt_version,
                  s->cfg.i_frequency, s->cfg.i_bandwidth, s->i_snr, s->cfg.i_modulation );
        if( !s->cfg.i_fec )
            s->cfg.i_fec = 9;   /* FEC_AUTO */
        char *psz;
        if( asprintf( &psz, "#EXTINF:,,%s\n"
                        "#EXTVLCOPT:program=%d\n"
                        "dvb://frequency=%d:bandwidth=%d:voltage=%d:fec=%d:modulation=%d:srate=%d\n"
                        "\n",
                      s->psz_name && * s->psz_name ? s->psz_name : "Unknown",
                      s->i_program,
                      s->cfg.i_frequency,
                      s->cfg.i_bandwidth,
                      s->cfg.c_polarization == 'H' ? 18 : 13,
                      s->cfg.i_fec,
                      s->cfg.i_modulation,
                      s->cfg.i_symbolrate ) < 0 )
            psz = NULL;
        if( psz )
        {
            block_t *p_block = BlockString( psz );
            if( p_block )
                block_ChainAppend( &p_playlist, p_block );
        }
    }
    return p_playlist ? block_ChainGather( p_playlist ) : NULL;
}
