 *****************************************************************************/
static void CAPMTAdd( cam_t * p_cam, int i_session_id,
                      dvbpsi_pmt_t *p_pmt )
{
    uint8_t *p_capmt;
    int i_capmt_size;
    if( p_cam->i_selected_programs >= CAM_PROG_MAX )
    {
        msg_Warn( p_cam->obj, "Not adding CAPMT for SID %d, too many programs",
                  p_pmt->i_program_number );
        return;
    }
    p_cam->i_selected_programs++;
    if( p_cam->i_selected_programs == 1 )
    {
        CAPMTFirst( p_cam, i_session_id, p_pmt );
        return;
    }
#ifdef CAPMT_WAIT
    msleep( CAPMT_WAIT * 1000 );
#endif
    msg_Dbg( p_cam->obj, "adding CAPMT for SID %d on session %d",
             p_pmt->i_program_number, i_session_id );
    p_capmt = CAPMTBuild( p_cam, i_session_id, p_pmt,
                          0x4 /* add */, 0x1 /* ok_descrambling */,
                          &i_capmt_size );
    if( i_capmt_size )
    {
        APDUSend( p_cam, i_session_id, AOT_CA_PMT, p_capmt, i_capmt_size );
        free( p_capmt );
    }
}
