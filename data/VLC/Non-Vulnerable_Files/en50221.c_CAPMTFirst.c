 *****************************************************************************/
static void CAPMTFirst( cam_t * p_cam, int i_session_id,
                        dvbpsi_pmt_t *p_pmt )
{
    uint8_t *p_capmt;
    int i_capmt_size;
    msg_Dbg( p_cam->obj, "adding first CAPMT for SID %d on session %d",
             p_pmt->i_program_number, i_session_id );
    p_capmt = CAPMTBuild( p_cam, i_session_id, p_pmt,
                          0x3 /* only */, 0x1 /* ok_descrambling */,
                          &i_capmt_size );
    if( i_capmt_size )
    {
        APDUSend( p_cam, i_session_id, AOT_CA_PMT, p_capmt, i_capmt_size );
        free( p_capmt );
    }
}
