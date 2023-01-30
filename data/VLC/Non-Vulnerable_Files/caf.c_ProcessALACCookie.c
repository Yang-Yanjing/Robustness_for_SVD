 */
static int ProcessALACCookie( demux_t *p_demux, const uint8_t *p, uint64_t i_size )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const unsigned int kALAC_NEW_KUKI_SIZE = 24;
    const unsigned int kALAC_LIB_REQ_KUKI_SIZE = 36;
    int i_extra;
    if( i_size == kALAC_NEW_KUKI_SIZE || i_size == kALAC_LIB_REQ_KUKI_SIZE )
    {
        i_extra = kALAC_LIB_REQ_KUKI_SIZE;
    }
    else
    {
        msg_Warn( p_demux, "Unknown alac magic cookie. Passing it on to the decoder as is and hoping for the best." );
        i_extra = ( int )i_size;
    }
    p_sys->fmt.i_extra = i_extra;
    p_sys->fmt.p_extra = malloc( i_extra );
    if( !p_sys->fmt.p_extra )
        return VLC_ENOMEM;
    uint8_t *p_extra = ( uint8_t * )p_sys->fmt.p_extra;
    if( i_size == kALAC_NEW_KUKI_SIZE )
    {
        SetDWBE( p_extra, 36 );
        memcpy( p_extra + 4, "alac", 4 );
        SetDWBE( p_extra + 8, 0 );
        memcpy( p_extra + 12, p, 24 );
    }
    else
    {
        memcpy( p_sys->fmt.p_extra, p, i_size );
    }
    return VLC_SUCCESS;
}
