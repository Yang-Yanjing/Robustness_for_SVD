}
vlc_fourcc_t vlc_fourcc_GetCodecAudio( vlc_fourcc_t i_fourcc, int i_bits )
{
    const int i_bytes = ( i_bits + 7 ) / 8;
    if( i_fourcc == VLC_FOURCC( 'a', 'f', 'l', 't' ) )
    {
        switch( i_bytes )
        {
        case 4:
            return VLC_CODEC_FL32;
        case 8:
            return VLC_CODEC_FL64;
        default:
            return 0;
        }
    }
    else if( i_fourcc == VLC_FOURCC( 'a', 'r', 'a', 'w' ) )
    {
        switch( i_bytes )
        {
        case 1:
            return VLC_CODEC_U8;
        case 2:
            return VLC_CODEC_S16L;
        case 3:
            return VLC_CODEC_S24L;
            break;
        case 4:
            return VLC_CODEC_S32L;
        default:
            return 0;
        }
    }
    else if( i_fourcc == VLC_FOURCC( 't', 'w', 'o', 's' ) )
    {
        switch( i_bytes )
        {
        case 1:
            return VLC_CODEC_S8;
        case 2:
            return VLC_CODEC_S16B;
        case 3:
            return VLC_CODEC_S24B;
        case 4:
            return VLC_CODEC_S32B;
        default:
            return 0;
        }
    }
    else if( i_fourcc == VLC_FOURCC( 's', 'o', 'w', 't' ) )
    {
        switch( i_bytes )
        {
        case 1:
            return VLC_CODEC_S8;
        case 2:
            return VLC_CODEC_S16L;
        case 3:
            return VLC_CODEC_S24L;
        case 4:
            return VLC_CODEC_S32L;
        default:
            return 0;
        }
    }
    else
    {
        return vlc_fourcc_GetCodec( AUDIO_ES, i_fourcc );
    }
}
