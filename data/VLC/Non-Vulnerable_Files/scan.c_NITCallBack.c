#ifdef DVBPSI_USE_NIT
static void NITCallBack( scan_session_t *p_session, dvbpsi_nit_t *p_nit )
{
    vlc_object_t *p_obj = p_session->p_obj;
    access_t *p_access = (access_t*)p_obj;
    access_sys_t *p_sys = p_access->p_sys;
    scan_t *p_scan = p_sys->scan;
    msg_Dbg( p_obj, "NITCallBack" );
    msg_Dbg( p_obj, "new NIT network_id=%d version=%d current_next=%d",
             p_nit->i_network_id, p_nit->i_version, p_nit->b_current_next );
    /* */
    if( p_session->p_nit && p_session->p_nit->b_current_next )
    {
        dvbpsi_DeleteNIT( p_session->p_nit );
        p_session->p_nit = NULL;
    }
    if( p_session->p_nit )
    {
        dvbpsi_DeleteNIT( p_nit );
        return;
    }
    /* */
    p_session->p_nit = p_nit;
    dvbpsi_descriptor_t *p_dsc;
    for( p_dsc = p_nit->p_first_descriptor; p_dsc != NULL; p_dsc = p_dsc->p_next )
    {
        if( p_dsc->i_tag == 0x40 )
        {
            msg_Dbg( p_obj, "   * network name descriptor" );
            char str1[257];
            memcpy( str1, p_dsc->p_data, p_dsc->i_length );
            str1[p_dsc->i_length] = '\0';
            msg_Dbg( p_obj, "       * name %s", str1 );
        }
        else if( p_dsc->i_tag == 0x4a )
        {
            msg_Dbg( p_obj, "   * linkage descriptor" );
            uint16_t i_ts_id = GetWBE( &p_dsc->p_data[0] );
            uint16_t i_on_id = GetWBE( &p_dsc->p_data[2] );
            uint16_t i_service_id = GetWBE( &p_dsc->p_data[4] );
            int i_linkage_type = p_dsc->p_data[6];
            msg_Dbg( p_obj, "       * ts_id %d", i_ts_id );
            msg_Dbg( p_obj, "       * on_id %d", i_on_id );
            msg_Dbg( p_obj, "       * service_id %d", i_service_id );
            msg_Dbg( p_obj, "       * linkage_type %d", i_linkage_type );
        }
        else 
        {
            msg_Dbg( p_obj, "   * dsc 0x%x", p_dsc->i_tag );
        }
    }
    dvbpsi_nit_ts_t *p_ts;
    for( p_ts = p_nit->p_first_ts; p_ts != NULL; p_ts = p_ts->p_next )
    {
        msg_Dbg( p_obj, "   * ts ts_id=0x%x original_id=0x%x", p_ts->i_ts_id, p_ts->i_orig_network_id );
        uint32_t i_private_data_id = 0;
        dvbpsi_descriptor_t *p_dsc;
        scan_configuration_t cfg, *p_cfg = &cfg;
        memset(p_cfg,0,sizeof(*p_cfg));
        for( p_dsc = p_ts->p_first_descriptor; p_dsc != NULL; p_dsc = p_dsc->p_next )
        {
            if( p_dsc->i_tag == 0x41 )
            {
                msg_Dbg( p_obj, "       * service list descriptor" );
                for( int i = 0; i < p_dsc->i_length/3; i++ )
                {
                    uint16_t i_service_id = GetWBE( &p_dsc->p_data[3*i+0] );
                    uint8_t  i_service_type = p_dsc->p_data[3*i+2];
                    msg_Dbg( p_obj, "           * service_id=%d type=%d", i_service_id, i_service_type );
#ifdef _DVBPSI_DR_44_H_
                    if( (ScanFindService( p_scan, 0, i_service_id ) == NULL) &&
                         scan_service_type( i_service_type ) != SERVICE_UNKNOWN )
                    {
                       scan_service_t *s = scan_service_New( i_service_id, p_cfg );
                       s->type          = scan_service_type( i_service_type );
                       s->i_network_id  = p_nit->i_network_id;
                       s->i_nit_version = p_nit->i_version;
                       TAB_APPEND( p_scan->i_service, p_scan->pp_service, s );
                    }
#endif
                }
            }
            else if( p_dsc->i_tag == 0x5a )
            {
                dvbpsi_terr_deliv_sys_dr_t *p_t = dvbpsi_DecodeTerrDelivSysDr( p_dsc );
                msg_Dbg( p_obj, "       * terrestrial delivery system" );
                msg_Dbg( p_obj, "           * centre_frequency 0x%x", p_t->i_centre_frequency  );
                msg_Dbg( p_obj, "           * bandwidth %d", 8 - p_t->i_bandwidth );
                msg_Dbg( p_obj, "           * constellation %d", p_t->i_constellation );
                msg_Dbg( p_obj, "           * hierarchy %d", p_t->i_hierarchy_information );
                msg_Dbg( p_obj, "           * code_rate hp %d lp %d", p_t->i_code_rate_hp_stream, p_t->i_code_rate_lp_stream );
                msg_Dbg( p_obj, "           * guard_interval %d", p_t->i_guard_interval );
                msg_Dbg( p_obj, "           * transmission_mode %d", p_t->i_transmission_mode );
                msg_Dbg( p_obj, "           * other_frequency_flag %d", p_t->i_other_frequency_flag );
            }
#ifdef _DVBPSI_DR_44_H_
            else if( p_dsc->i_tag == 0x44 )
            {
                dvbpsi_cable_deliv_sys_dr_t *p_t = dvbpsi_DecodeCableDelivSysDr( p_dsc );
                msg_Dbg( p_obj, "       * Cable delivery system");
                p_cfg->i_frequency =  decode_BCD( p_t->i_frequency ) * 100;
                msg_Dbg( p_obj, "           * frequency %d", p_cfg->i_frequency );
                p_cfg->i_symbolrate =  decode_BCD( p_t->i_symbol_rate ) * 100;
                msg_Dbg( p_obj, "           * symbolrate %u", p_cfg->i_symbolrate );
                p_cfg->i_modulation = (8 << p_t->i_modulation);
                msg_Dbg( p_obj, "           * modulation %u", p_cfg->i_modulation );
            }
#endif
            else if( p_dsc->i_tag == 0x5f )
            {
                msg_Dbg( p_obj, "       * private data specifier descriptor" );
                i_private_data_id = GetDWBE( &p_dsc->p_data[0] );
                msg_Dbg( p_obj, "           * value 0x%8.8x", i_private_data_id );
            }
            else if( i_private_data_id == 0x28 && p_dsc->i_tag == 0x83 )
            {
                msg_Dbg( p_obj, "       * logical channel descriptor (EICTA)" );
                for( int i = 0; i < p_dsc->i_length/4; i++ )
                {
                    uint16_t i_service_id = GetWBE( &p_dsc->p_data[4*i+0] );
                    int i_channel_number = GetWBE( &p_dsc->p_data[4*i+2] ) & 0x3ff;
                    msg_Dbg( p_obj, "           * service_id=%d channel_number=%d", i_service_id, i_channel_number );
                    scan_service_t *s = ScanFindService( p_scan, 0, i_service_id );
                    if( s && s->i_channel < 0 ) s->i_channel = i_channel_number;
                }
            }
            else
            {
                msg_Warn( p_obj, "       * dsc 0x%x", p_dsc->i_tag );
            }
        }
    }
}
