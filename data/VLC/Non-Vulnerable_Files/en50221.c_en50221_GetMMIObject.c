 *****************************************************************************/
static mmi_t *en50221_GetMMIObject( cam_t * p_cam, unsigned i_slot )
{
    if( p_cam->pb_slot_mmi_expected[i_slot] )
        return NULL; /* should not happen */
    for( unsigned i = 1; i <= MAX_SESSIONS; i++ )
    {
        if ( p_cam->p_sessions[i - 1].i_resource_id == RI_MMI
              && p_cam->p_sessions[i - 1].i_slot == i_slot )
        {
            mmi_t *p_mmi =
                (mmi_t *)p_cam->p_sessions[i - 1].p_sys;
            if ( p_mmi == NULL )
                return NULL; /* should not happen */
            return p_mmi;
        }
    }
    return NULL;
}
