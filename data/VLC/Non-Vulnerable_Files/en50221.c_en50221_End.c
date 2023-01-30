 *****************************************************************************/
void en50221_End( cam_t * p_cam )
{
    for( unsigned i = 0; i < MAX_PROGRAMS; i++ )
    {
        if( p_cam->pp_selected_programs[i] != NULL )
        {
            dvbpsi_DeletePMT( p_cam->pp_selected_programs[i] );
        }
    }
    for( unsigned i = 1; i <= MAX_SESSIONS; i++ )
    {
        if( p_cam->p_sessions[i - 1].i_resource_id
              && p_cam->p_sessions[i - 1].pf_close != NULL )
        {
            p_cam->p_sessions[i - 1].pf_close( p_cam, i );
        }
    }
    close( p_cam->fd );
    free( p_cam );
}
