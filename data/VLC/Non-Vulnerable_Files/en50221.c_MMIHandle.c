 *****************************************************************************/
static void MMIHandle( cam_t *p_cam, int i_session_id,
                       uint8_t *p_apdu, int i_size )
{
    int i_tag = APDUGetTag( p_apdu, i_size );
    switch ( i_tag )
    {
    case AOT_DISPLAY_CONTROL:
    {
        int l;
        uint8_t *d = APDUGetLength( p_apdu, &l );
        if ( l > 0 )
        {
            switch ( *d )
            {
            case DCC_SET_MMI_MODE:
                if ( l == 2 && d[1] == MM_HIGH_LEVEL )
                    MMIDisplayReply( p_cam, i_session_id );
                else
                    msg_Err( p_cam->obj, "unsupported MMI mode %02x", d[1] );
                break;
            default:
                msg_Err( p_cam->obj, "unsupported display control command %02x",
                         *d );
                break;
            }
        }
        break;
    }
    case AOT_ENQ:
        MMIHandleEnq( p_cam, i_session_id, p_apdu, i_size );
        break;
    case AOT_LIST_LAST:
    case AOT_MENU_LAST:
        MMIHandleMenu( p_cam, i_session_id, i_tag, p_apdu, i_size );
        break;
    case AOT_CLOSE_MMI:
        SessionSendClose( p_cam, i_session_id );
        break;
    default:
        msg_Err( p_cam->obj, "unexpected tag in MMIHandle (0x%x)", i_tag );
    }
}
