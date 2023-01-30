#endif
scan_session_t *scan_session_New( vlc_object_t *p_obj,
                                  const scan_configuration_t *p_cfg )
{
    scan_session_t *p_session = malloc( sizeof( *p_session ) );
    if( unlikely(p_session == NULL) )
        return NULL;
    p_session->p_obj = p_obj;
    p_session->cfg = *p_cfg;
    p_session->i_snr = -1;
    p_session->pat = NULL;
    p_session->p_pat = NULL;
    p_session->i_nit_pid = -1;
    p_session->sdt = NULL;
    p_session->p_sdt = NULL;
#ifdef DVBPSI_USE_NIT
    p_session->nit = NULL;
    p_session->p_nit = NULL;
#endif
    return p_session;;
}
