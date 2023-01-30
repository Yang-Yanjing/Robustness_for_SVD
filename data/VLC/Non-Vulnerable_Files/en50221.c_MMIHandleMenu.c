 *****************************************************************************/
static void MMIHandleMenu( cam_t *p_cam, int i_session_id, int i_tag,
                           uint8_t *p_apdu, int i_size )
{
    VLC_UNUSED(i_size);
    mmi_t *p_mmi = (mmi_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    int i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    int l;
    uint8_t *d = APDUGetLength( p_apdu, &l );
    MMIFree( p_mmi );
    p_mmi->i_object_type = (i_tag == AOT_MENU_LAST) ?
                                       EN50221_MMI_MENU : EN50221_MMI_LIST;
    p_mmi->u.menu.i_choices = 0;
    p_mmi->u.menu.ppsz_choices = NULL;
    if ( l > 0 )
    {
        l--; d++; /* choice_nb */
#define GET_FIELD( x )                                                      \
        if ( l > 0 )                                                        \
        {                                                                   \
            p_mmi->u.menu.psz_##x = MMIGetText( p_cam, &d, &l );            \
            msg_Dbg( p_cam->obj, "MMI " STRINGIFY( x ) ": %s",              \
                     p_mmi->u.menu.psz_##x );                               \
        }
        GET_FIELD( title );
        GET_FIELD( subtitle );
        GET_FIELD( bottom );
#undef GET_FIELD
        while ( l > 0 )
        {
            char *psz_text = MMIGetText( p_cam, &d, &l );
            TAB_APPEND( p_mmi->u.menu.i_choices,
                        p_mmi->u.menu.ppsz_choices,
                        psz_text );
            msg_Dbg( p_cam->obj, "MMI choice: %s", psz_text );
        }
    }
    p_cam->pb_slot_mmi_expected[i_slot] = false;
    p_cam->pb_slot_mmi_undisplayed[i_slot] = true;
}
