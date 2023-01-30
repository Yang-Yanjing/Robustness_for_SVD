 *****************************************************************************/
static void SessionCreateResponse( cam_t * p_cam, uint8_t i_slot,
                                   uint8_t *p_spdu, int i_size )
{
    VLC_UNUSED( i_size );
    VLC_UNUSED( i_slot );
    int i_status = p_spdu[2];
    int i_resource_id = ResourceIdToInt( &p_spdu[3] );
    int i_session_id = ((int)p_spdu[7] << 8) | p_spdu[8];
    if ( i_status != SS_OK )
    {
        msg_Err( p_cam->obj, "SessionCreateResponse: failed to open session %d"
                 " resource=0x%x status=0x%x", i_session_id, i_resource_id,
                 i_status );
        p_cam->p_sessions[i_session_id - 1].i_resource_id = 0;
        return;
    }
    switch ( i_resource_id )
    {
    case RI_RESOURCE_MANAGER:
        ResourceManagerOpen( p_cam, i_session_id ); break;
    case RI_APPLICATION_INFORMATION:
        ApplicationInformationOpen( p_cam, i_session_id ); break;
    case RI_CONDITIONAL_ACCESS_SUPPORT:
        ConditionalAccessOpen( p_cam, i_session_id ); break;
    case RI_DATE_TIME:
        DateTimeOpen( p_cam, i_session_id ); break;
    case RI_MMI:
        MMIOpen( p_cam, i_session_id ); break;
    case RI_HOST_CONTROL:
    default:
        msg_Err( p_cam->obj, "unknown resource id (0x%x)", i_resource_id );
        p_cam->p_sessions[i_session_id - 1].i_resource_id = 0;
    }
}
