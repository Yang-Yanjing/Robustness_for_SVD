/* Checks if current packet matches codec keyframe */
bool Ogg_IsKeyFrame( logical_stream_t *p_stream, ogg_packet *p_packet )
{
    if ( p_stream->b_oggds )
    {
        return ( p_packet->bytes > 0 && p_packet->packet[0] & PACKET_IS_SYNCPOINT );
    }
    else switch ( p_stream->fmt.i_codec )
    {
    case VLC_CODEC_THEORA:
        if ( p_packet->bytes <= 0 || p_packet->packet[0] & THEORA_FTYPE_NOTDATA )
            return false;
        else
            return !( p_packet->packet[0] & THEORA_FTYPE_INTERFRAME );
    case VLC_CODEC_VP8:
        return ( ( ( p_packet->granulepos >> 3 ) & 0x07FFFFFF ) == 0 );
    case VLC_CODEC_DIRAC:
        return ( p_packet->granulepos & 0xFF8000FF );
    default:
        return true;
    }
}
