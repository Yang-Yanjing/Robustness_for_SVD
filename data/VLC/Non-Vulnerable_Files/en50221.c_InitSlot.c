#define MAX_TC_RETRIES 20
static int InitSlot( cam_t * p_cam, int i_slot )
{
    if ( TPDUSend( p_cam, i_slot, T_CREATE_TC, NULL, 0 ) != VLC_SUCCESS )
    {
        msg_Err( p_cam->obj, "en50221_Init: couldn't send TPDU on slot %d",
                 i_slot );
        return VLC_EGENERIC;
    }
    /* This is out of the spec */
    for ( int i = 0; i < MAX_TC_RETRIES; i++ )
    {
        uint8_t i_tag;
        if ( TPDURecv( p_cam, i_slot, &i_tag, NULL, NULL ) == VLC_SUCCESS
              && i_tag == T_CTC_REPLY )
        {
            p_cam->pb_active_slot[i_slot] = true;
            break;
        }
        if ( TPDUSend( p_cam, i_slot, T_CREATE_TC, NULL, 0 )
                != VLC_SUCCESS )
        {
            msg_Err( p_cam->obj,
                     "en50221_Init: couldn't send TPDU on slot %d",
                     i_slot );
            continue;
        }
    }
    if ( p_cam->pb_active_slot[i_slot] )
    {
        p_cam->i_timeout = CLOCK_FREQ / 10;
        return VLC_SUCCESS;
    }
    return VLC_EGENERIC;
}
