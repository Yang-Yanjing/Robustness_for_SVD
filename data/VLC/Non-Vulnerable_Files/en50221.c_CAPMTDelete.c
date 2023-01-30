 *****************************************************************************/
static void CAPMTDelete( cam_t * p_cam, int i_session_id,
                         dvbpsi_pmt_t *p_pmt )
{
    uint8_t *p_capmt;
    int i_capmt_size;
    p_cam->i_selected_programs--;
    msg_Dbg( p_cam->obj, "deleting CAPMT for SID %d on session %d",
             p_pmt->i_program_number, i_session_id );
    p_capmt = CAPMTBuild( p_cam, i_session_id, p_pmt,
                          0x5 /* update */, 0x4 /* not selected */,
                          &i_capmt_size );
    if( i_capmt_size )
    {
        APDUSend( p_cam, i_session_id, AOT_CA_PMT, p_capmt, i_capmt_size );
        free( p_capmt );
    }
}
