 *****************************************************************************/
static int en50221_OpenMMI( cam_t * p_cam, unsigned i_slot )
{
    if( p_cam->i_ca_type & CA_CI_LINK )
    {
        for ( unsigned i = 1; i <= MAX_SESSIONS; i++ )
        {
            if ( p_cam->p_sessions[i - 1].i_resource_id == RI_MMI
                  && p_cam->p_sessions[i - 1].i_slot == i_slot )
            {
                msg_Dbg( p_cam->obj,
                         "MMI menu is already opened on slot %d (session=%u)",
                         i_slot, i );
                return VLC_SUCCESS;
            }
        }
        for ( unsigned i = 1; i <= MAX_SESSIONS; i++ )
        {
            if ( p_cam->p_sessions[i - 1].i_resource_id
                    == RI_APPLICATION_INFORMATION
                  && p_cam->p_sessions[i - 1].i_slot == i_slot )
            {
                ApplicationInformationEnterMenu( p_cam, i );
                return VLC_SUCCESS;
            }
        }
        msg_Err( p_cam->obj, "no application information on slot %d", i_slot );
        return VLC_EGENERIC;
    }
    else
    {
        msg_Err( p_cam->obj, "MMI menu not supported" );
        return VLC_EGENERIC;
    }
}
