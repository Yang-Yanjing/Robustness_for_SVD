}
static uint8_t *CAPMTBuild( cam_t * p_cam, int i_session_id,
                            dvbpsi_pmt_t *p_pmt, uint8_t i_list_mgt,
                            uint8_t i_cmd, int *pi_capmt_size )
{
    system_ids_t *p_ids =
        (system_ids_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    dvbpsi_pmt_es_t *p_es;
    int i_cad_size, i_cad_program_size;
    uint8_t *p_capmt;
    i_cad_size = i_cad_program_size =
            GetCADSize( p_ids, p_pmt->p_first_descriptor );
    for( p_es = p_pmt->p_first_es; p_es != NULL; p_es = p_es->p_next )
    {
        i_cad_size += GetCADSize( p_ids, p_es->p_first_descriptor );
    }
    if ( !i_cad_size )
    {
        msg_Warn( p_cam->obj,
                  "no compatible scrambling system for SID %d on session %d",
                  p_pmt->i_program_number, i_session_id );
        *pi_capmt_size = 0;
        return NULL;
    }
    p_capmt = CAPMTHeader( p_ids, i_list_mgt, p_pmt->i_program_number,
                           p_pmt->i_version, i_cad_program_size,
                           p_pmt->p_first_descriptor, i_cmd );
    if ( i_cad_program_size )
        *pi_capmt_size = 7 + i_cad_program_size;
    else
        *pi_capmt_size = 6;
    for( p_es = p_pmt->p_first_es; p_es != NULL; p_es = p_es->p_next )
    {
        i_cad_size = GetCADSize( p_ids, p_es->p_first_descriptor );
        if ( i_cad_size || i_cad_program_size )
        {
            p_capmt = CAPMTES( p_ids, p_capmt, *pi_capmt_size, p_es->i_type,
                               p_es->i_pid, i_cad_size,
                               p_es->p_first_descriptor, i_cmd );
            if ( i_cad_size )
                *pi_capmt_size += 6 + i_cad_size;
            else
                *pi_capmt_size += 5;
        }
    }
    return p_capmt;
}
