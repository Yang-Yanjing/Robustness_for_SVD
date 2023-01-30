 *****************************************************************************/
static int en50221_CloseMMI( cam_t * p_cam, unsigned i_slot )
{
    if( p_cam->i_ca_type & CA_CI_LINK )
    {
        for( unsigned i = 1; i <= MAX_SESSIONS; i++ )
        {
            if ( p_cam->p_sessions[i - 1].i_resource_id == RI_MMI
                  && p_cam->p_sessions[i - 1].i_slot == i_slot )
            {
                MMISendClose( p_cam, i );
                return VLC_SUCCESS;
            }
        }
        msg_Warn( p_cam->obj, "closing a non-existing MMI session on slot %d",
                  i_slot );
        return VLC_EGENERIC;
    }
    else
    {
        msg_Err( p_cam->obj, "MMI menu not supported" );
        return VLC_EGENERIC;
    }
}
