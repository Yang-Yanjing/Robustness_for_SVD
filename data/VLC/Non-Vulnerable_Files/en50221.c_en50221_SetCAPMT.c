 *****************************************************************************/
int en50221_SetCAPMT( cam_t * p_cam, dvbpsi_pmt_t *p_pmt )
{
    bool b_update = false;
    bool b_needs_descrambling = CAPMTNeedsDescrambling( p_pmt );
    for ( unsigned i = 0; i < MAX_PROGRAMS; i++ )
    {
        if ( p_cam->pp_selected_programs[i] != NULL
              && p_cam->pp_selected_programs[i]->i_program_number
                  == p_pmt->i_program_number )
        {
            b_update = true;
            if ( !b_needs_descrambling )
            {
                dvbpsi_DeletePMT( p_pmt );
                p_pmt = p_cam->pp_selected_programs[i];
                p_cam->pp_selected_programs[i] = NULL;
            }
            else if( p_pmt != p_cam->pp_selected_programs[i] )
            {
                dvbpsi_DeletePMT( p_cam->pp_selected_programs[i] );
                p_cam->pp_selected_programs[i] = p_pmt;
            }
            break;
        }
    }
    if ( !b_update && b_needs_descrambling )
    {
        for ( unsigned i = 0; i < MAX_PROGRAMS; i++ )
        {
            if ( p_cam->pp_selected_programs[i] == NULL )
            {
                p_cam->pp_selected_programs[i] = p_pmt;
                break;
            }
        }
    }
    if ( b_update || b_needs_descrambling )
    {
        for ( unsigned i = 1; i <= MAX_SESSIONS; i++ )
        {
            if ( p_cam->p_sessions[i - 1].i_resource_id
                    == RI_CONDITIONAL_ACCESS_SUPPORT )
            {
                if ( b_update && b_needs_descrambling )
                    CAPMTUpdate( p_cam, i, p_pmt );
                else if ( b_update )
                    CAPMTDelete( p_cam, i, p_pmt );
                else
                    CAPMTAdd( p_cam, i, p_pmt );
            }
        }
    }
    if ( !b_needs_descrambling )
    {
        dvbpsi_DeletePMT( p_pmt );
    }
    return VLC_SUCCESS;
}
