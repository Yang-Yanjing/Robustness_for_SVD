 *****************************************************************************/
static void en50221_SendMMIObject( cam_t * p_cam, unsigned i_slot,
                                   mmi_t *p_object )
{
    for( unsigned i = 1; i <= MAX_SESSIONS; i++ )
    {
        if ( p_cam->p_sessions[i - 1].i_resource_id == RI_MMI
              && p_cam->p_sessions[i - 1].i_slot == i_slot )
        {
            MMISendObject( p_cam, i, p_object );
            return;
        }
    }
    msg_Err( p_cam->obj, "SendMMIObject when no MMI session is opened !" );
}
