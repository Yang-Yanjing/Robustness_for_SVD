}
static void SDTCallBack( scan_session_t *p_session, dvbpsi_sdt_t *p_sdt )
{
    vlc_object_t *p_obj = p_session->p_obj;
    msg_Dbg( p_obj, "SDTCallBack" );
    if( p_session->p_sdt && p_session->p_sdt->b_current_next )
    {
        dvbpsi_DeleteSDT( p_session->p_sdt );
        p_session->p_sdt = NULL;
    }
    if( p_session->p_sdt )
    {
        dvbpsi_DeleteSDT( p_sdt );
        return;
    }
    /* */
    p_session->p_sdt = p_sdt;
    /* */
    msg_Dbg( p_obj, "new SDT ts_id=%d version=%d current_next=%d network_id=%d",
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
             p_sdt->i_extension,
#else
             p_sdt->i_ts_id,
#endif
             p_sdt->i_version, p_sdt->b_current_next,
             p_sdt->i_network_id );
    dvbpsi_sdt_service_t *p_srv;
    for( p_srv = p_sdt->p_first_service; p_srv; p_srv = p_srv->p_next )
    {
        dvbpsi_descriptor_t *p_dr;
        msg_Dbg( p_obj, "  * service id=%d eit schedule=%d present=%d running=%d free_ca=%d",
                 p_srv->i_service_id, p_srv->b_eit_schedule,
                 p_srv->b_eit_present, p_srv->i_running_status,
                 p_srv->b_free_ca );
        for( p_dr = p_srv->p_first_descriptor; p_dr; p_dr = p_dr->p_next )
        {
            if( p_dr->i_tag == 0x48 )
            {
                dvbpsi_service_dr_t *pD = dvbpsi_DecodeServiceDr( p_dr );
                char str2[257];
                memcpy( str2, pD->i_service_name, pD->i_service_name_length );
                str2[pD->i_service_name_length] = '\0';
                msg_Dbg( p_obj, "    - type=%d name=%s",
                         pD->i_service_type, str2 );
            }
            else
            {
                msg_Dbg( p_obj, "    * dsc 0x%x", p_dr->i_tag );
            }
        }
    }
}
