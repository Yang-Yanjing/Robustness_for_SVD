 *****************************************************************************/
static int PacketizerOpen( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    if( DecoderOpen( p_this ) != VLC_SUCCESS ) return VLC_EGENERIC;
    p_dec->p_sys->b_packetizer = true;
    return VLC_SUCCESS;
}
