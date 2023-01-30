 *****************************************************************************/
static int SPDUSend( cam_t * p_cam, int i_session_id,
                     uint8_t *p_data, int i_size )
{
    uint8_t *p_spdu = xmalloc( i_size + 4 );
    uint8_t *p = p_spdu;
    uint8_t i_tag;
    uint8_t i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    *p++ = ST_SESSION_NUMBER;
    *p++ = 0x02;
    *p++ = (i_session_id >> 8);
    *p++ = i_session_id & 0xff;
    memcpy( p, p_data, i_size );
    i_size += 4;
    p = p_spdu;
    while ( i_size > 0 )
    {
        if ( i_size > MAX_TPDU_DATA )
        {
            if ( TPDUSend( p_cam, i_slot, T_DATA_MORE, p,
                           MAX_TPDU_DATA ) != VLC_SUCCESS )
            {
                msg_Err( p_cam->obj, "couldn't send TPDU on session %d",
                         i_session_id );
                free( p_spdu );
                return VLC_EGENERIC;
            }
            p += MAX_TPDU_DATA;
            i_size -= MAX_TPDU_DATA;
        }
        else
        {
            if ( TPDUSend( p_cam, i_slot, T_DATA_LAST, p, i_size )
                    != VLC_SUCCESS )
            {
                msg_Err( p_cam->obj, "couldn't send TPDU on session %d",
                         i_session_id );
                free( p_spdu );
                return VLC_EGENERIC;
            }
            i_size = 0;
        }
        if ( TPDURecv( p_cam, i_slot, &i_tag, NULL, NULL ) != VLC_SUCCESS
               || i_tag != T_SB )
        {
            msg_Err( p_cam->obj, "couldn't recv TPDU on session %d",
                     i_session_id );
            free( p_spdu );
            return VLC_EGENERIC;
        }
    }
    free( p_spdu );
    return VLC_SUCCESS;
}
