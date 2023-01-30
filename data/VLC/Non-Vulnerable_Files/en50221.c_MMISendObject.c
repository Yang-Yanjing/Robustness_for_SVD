 *****************************************************************************/
static void MMISendObject( cam_t *p_cam, int i_session_id,
                           mmi_t *p_object )
{
    int i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    uint8_t *p_data;
    int i_size, i_tag;
    switch ( p_object->i_object_type )
    {
    case EN50221_MMI_ANSW:
        i_tag = AOT_ANSW;
        i_size = 1 + strlen( p_object->u.answ.psz_answ );
        p_data = xmalloc( i_size );
        p_data[0] = p_object->u.answ.b_ok ? 0x1 : 0x0;
        strncpy( (char *)&p_data[1], p_object->u.answ.psz_answ, i_size - 1 );
        break;
    case EN50221_MMI_MENU_ANSW:
        i_tag = AOT_MENU_ANSW;
        i_size = 1;
        p_data = xmalloc( i_size );
        p_data[0] = p_object->u.menu_answ.i_choice;
        break;
    default:
        msg_Err( p_cam->obj, "unknown MMI object %d", p_object->i_object_type );
        return;
    }
    APDUSend( p_cam, i_session_id, i_tag, p_data, i_size );
    free( p_data );
    p_cam->pb_slot_mmi_expected[i_slot] = true;
}
