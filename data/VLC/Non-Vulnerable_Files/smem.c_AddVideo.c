}
static sout_stream_id_sys_t *AddVideo( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    char* psz_tmp;
    sout_stream_id_sys_t    *id;
    int i_bits_per_pixel;
    switch( p_fmt->i_codec )
    {
        case VLC_CODEC_RGB32:
        case VLC_CODEC_RGBA:
        case VLC_CODEC_ARGB:
            i_bits_per_pixel = 32;
            break;
        case VLC_CODEC_I444:
        case VLC_CODEC_RGB24:
            i_bits_per_pixel = 24;
            break;
        case VLC_CODEC_RGB16:
        case VLC_CODEC_RGB15:
        case VLC_CODEC_RGB8:
        case VLC_CODEC_I422:
            i_bits_per_pixel = 16;
            break;
        case VLC_CODEC_YV12:
        case VLC_CODEC_I420:
            i_bits_per_pixel = 12;
            break;
        case VLC_CODEC_RGBP:
            i_bits_per_pixel = 8;
            break;
        default:
            i_bits_per_pixel = 0;
            msg_Dbg( p_stream, "non raw video format detected (%4.4s), buffers will contain compressed video", (char *)&p_fmt->i_codec );
            break;
    }
    id = calloc( 1, sizeof( sout_stream_id_sys_t ) );
    if( !id )
        return NULL;
    psz_tmp = var_GetString( p_stream, SOUT_PREFIX_VIDEO "data" );
    id->p_data = (void *)( intptr_t )atoll( psz_tmp );
    free( psz_tmp );
    id->format = p_fmt;
    id->format->video.i_bits_per_pixel = i_bits_per_pixel;
    return id;
}
