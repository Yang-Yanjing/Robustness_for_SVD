}
static int OpenPacketizer( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    int ret = OpenCommon( p_dec );
    if( ret == VLC_SUCCESS )
        p_dec->pf_packetize = SendFrame;
    return ret;
}
