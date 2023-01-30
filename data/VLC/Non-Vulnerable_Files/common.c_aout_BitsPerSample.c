 *****************************************************************************/
unsigned int aout_BitsPerSample( vlc_fourcc_t i_format )
{
    switch( vlc_fourcc_GetCodec( AUDIO_ES, i_format ) )
    {
    case VLC_CODEC_U8:
    case VLC_CODEC_S8:
    case VLC_CODEC_ALAW:
    case VLC_CODEC_MULAW:
        return 8;
    case VLC_CODEC_U16L:
    case VLC_CODEC_S16L:
    case VLC_CODEC_U16B:
    case VLC_CODEC_S16B:
        return 16;
    case VLC_CODEC_U24L:
    case VLC_CODEC_S24L:
    case VLC_CODEC_U24B:
    case VLC_CODEC_S24B:
        return 24;
    case VLC_CODEC_S24L32:
    case VLC_CODEC_S24B32:
    case VLC_CODEC_U32L:
    case VLC_CODEC_U32B:
    case VLC_CODEC_S32L:
    case VLC_CODEC_S32B:
    case VLC_CODEC_F32L:
    case VLC_CODEC_F32B:
        return 32;
    case VLC_CODEC_F64L:
    case VLC_CODEC_F64B:
        return 64;
    default:
        /* For these formats the caller has to indicate the parameters
         * by hand. */
        return 0;
    }
}
