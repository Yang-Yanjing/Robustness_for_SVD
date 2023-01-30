#undef DEFINE_REMAP
static inline remap_fun_t GetRemapFun( audio_format_t *p_format, bool b_add )
{
    if( b_add )
    {
        switch( p_format->i_format )
        {
            case VLC_CODEC_U8:
                return RemapAddU8;
            case VLC_CODEC_S16N:
                return RemapAddS16N;
            case VLC_CODEC_S32N:
                return RemapAddS32N;
            case VLC_CODEC_FL32:
                return RemapAddFL32;
            case VLC_CODEC_FL64:
                return RemapAddFL64;
        }
    }
    else
    {
        switch( p_format->i_format )
        {
            case VLC_CODEC_U8:
                return RemapCopyU8;
            case VLC_CODEC_S16N:
                return RemapCopyS16N;
            case VLC_CODEC_S32N:
                return RemapCopyS32N;
            case VLC_CODEC_FL32:
                return RemapCopyFL32;
            case VLC_CODEC_FL64:
                return RemapCopyFL64;
        }
    }
    return NULL;
}
