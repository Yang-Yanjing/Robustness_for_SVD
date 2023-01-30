 *****************************************************************************/
static void MMIHandleEnq( cam_t *p_cam, int i_session_id,
                          uint8_t *p_apdu, int i_size )
{
    VLC_UNUSED( i_size );
    mmi_t *p_mmi = (mmi_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    int i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    int l;
    uint8_t *d = APDUGetLength( p_apdu, &l );
    MMIFree( p_mmi );
    p_mmi->i_object_type = EN50221_MMI_ENQ;
    p_mmi->u.enq.b_blind = (*d & 0x1) ? true : false;
    d += 2; /* skip answer_text_length because it is not mandatory */
    l -= 2;
    p_mmi->u.enq.psz_text = xmalloc( l + 1 );
    strncpy( p_mmi->u.enq.psz_text, (char *)d, l );
    p_mmi->u.enq.psz_text[l] = '\0';
    msg_Dbg( p_cam->obj, "MMI enq: %s%s", p_mmi->u.enq.psz_text,
             p_mmi->u.enq.b_blind ? " (blind)" : "" );
    p_cam->pb_slot_mmi_expected[i_slot] = false;
    p_cam->pb_slot_mmi_undisplayed[i_slot] = true;
}
