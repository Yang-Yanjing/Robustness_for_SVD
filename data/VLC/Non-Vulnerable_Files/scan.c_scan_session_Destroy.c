}
void scan_session_Destroy( scan_t *p_scan, scan_session_t *p_session )
{
    const int i_service_start = p_scan->i_service;
    dvbpsi_pat_t *p_pat = p_session->p_pat;
    dvbpsi_sdt_t *p_sdt = p_session->p_sdt;
#ifdef DVBPSI_USE_NIT
    dvbpsi_nit_t *p_nit = p_session->p_nit;
#endif
    if( p_pat )
    {
        /* Parse PAT */
        dvbpsi_pat_program_t *p_program;
        for( p_program = p_pat->p_first_program; p_program != NULL; p_program = p_program->p_next )
        {
            if( p_program->i_number == 0 )  /* NIT */
                continue;
            scan_service_t *s = ScanFindService( p_scan, 0, p_program->i_number );
            if( s == NULL )
            {
                s = scan_service_New( p_program->i_number, &p_session->cfg );
                TAB_APPEND( p_scan->i_service, p_scan->pp_service, s );
            }
        }
    }
    /* Parse SDT */
    if( p_pat && p_sdt )
    {
        dvbpsi_sdt_service_t *p_srv;
        for( p_srv = p_sdt->p_first_service; p_srv; p_srv = p_srv->p_next )
        {
            scan_service_t *s = ScanFindService( p_scan, 0, p_srv->i_service_id );
            dvbpsi_descriptor_t *p_dr;
            if( s )
                s->b_crypted = p_srv->b_free_ca;
            for( p_dr = p_srv->p_first_descriptor; p_dr; p_dr = p_dr->p_next )
            {
                if( p_dr->i_tag == 0x48 )
                {
                    dvbpsi_service_dr_t *pD = dvbpsi_DecodeServiceDr( p_dr );
                    if( s )
                    {
                        if( !s->psz_name )
                            s->psz_name = vlc_from_EIT( pD->i_service_name,
                                                   pD->i_service_name_length );
                        if( s->type == SERVICE_UNKNOWN )
                            s->type = scan_service_type( pD->i_service_type );
                    }
                }
            }
        }
    }
#ifdef DVBPSI_USE_NIT
    /* Parse NIT */
    if( p_pat && p_nit )
    {
        dvbpsi_nit_ts_t *p_ts;
        for( p_ts = p_nit->p_first_ts; p_ts != NULL; p_ts = p_ts->p_next )
        {
            uint32_t i_private_data_id = 0;
            dvbpsi_descriptor_t *p_dsc;
            if( p_ts->i_orig_network_id != p_nit->i_network_id || p_ts->i_ts_id != p_pat->i_ts_id )
                continue;
            for( p_dsc = p_ts->p_first_descriptor; p_dsc != NULL; p_dsc = p_dsc->p_next )
            {
                if( p_dsc->i_tag == 0x5f )
                {
                    i_private_data_id = GetDWBE( &p_dsc->p_data[0] );
                }
                else if( i_private_data_id == 0x28 && p_dsc->i_tag == 0x83 )
                {
                    for( int i = 0; i < p_dsc->i_length/4; i++ )
                    {
                        uint16_t i_service_id = GetWBE( &p_dsc->p_data[4*i+0] );
                        int i_channel_number = GetWBE( &p_dsc->p_data[4*i+2] ) & 0x3ff;
                        scan_service_t *s = ScanFindService( p_scan, i_service_start, i_service_id );
                        if( s && s->i_channel < 0 )
                            s->i_channel = i_channel_number;
                    }
                }
            }
        }
    }
#endif
    /* */
    for( int i = i_service_start; i < p_scan->i_service; i++ )
    {
        scan_service_t *p_srv = p_scan->pp_service[i];
        p_srv->i_snr = p_session->i_snr;
        if( p_sdt )
            p_srv->i_sdt_version = p_sdt->i_version;
#ifdef DVBPSI_USE_NIT
        if( p_nit )
        {
            p_srv->i_network_id = p_nit->i_network_id;
            p_srv->i_nit_version = p_nit->i_version;
        }
#endif
    }
    /* */
    if( p_session->pat )
        dvbpsi_DetachPAT( p_session->pat );
    if( p_session->p_pat )
        dvbpsi_DeletePAT( p_session->p_pat );
    if( p_session->sdt )
        dvbpsi_DetachDemux( p_session->sdt );
    if( p_session->p_sdt )
        dvbpsi_DeleteSDT( p_session->p_sdt );
#ifdef DVBPSI_USE_NIT
    if( p_session->nit )
        dvbpsi_DetachDemux( p_session->nit );
    if( p_session->p_nit )
        dvbpsi_DeleteNIT( p_session->p_nit );
#endif
    free( p_session );
}
