 *****************************************************************************/
static void ConditionalAccessHandle( cam_t * p_cam, int i_session_id,
                                     uint8_t *p_apdu, int i_size )
{
    system_ids_t *p_ids =
        (system_ids_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    int i_tag = APDUGetTag( p_apdu, i_size );
    switch ( i_tag )
    {
    case AOT_CA_INFO:
    {
        int i;
        int l = 0;
        uint8_t *d = APDUGetLength( p_apdu, &l );
        msg_Dbg( p_cam->obj, "CA system IDs supported by the application :" );
        for ( i = 0; i < l / 2; i++ )
        {
            p_ids->pi_system_ids[i] = ((uint16_t)d[0] << 8) | d[1];
            d += 2;
            msg_Dbg( p_cam->obj, "- 0x%x", p_ids->pi_system_ids[i] );
        }
        p_ids->pi_system_ids[i] = 0;
        for ( i = 0; i < MAX_PROGRAMS; i++ )
        {
            if ( p_cam->pp_selected_programs[i] != NULL )
            {
                CAPMTAdd( p_cam, i_session_id,
                          p_cam->pp_selected_programs[i] );
            }
        }
        break;
    }
    default:
        msg_Err( p_cam->obj,
                 "unexpected tag in ConditionalAccessHandle (0x%x)",
                 i_tag );
    }
}
