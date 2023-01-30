}
void video_format_Setup( video_format_t *p_fmt, vlc_fourcc_t i_chroma,
                         int i_width, int i_height,
                         int i_visible_width, int i_visible_height,
                         int i_sar_num, int i_sar_den )
{
    p_fmt->i_chroma         = vlc_fourcc_GetCodec( VIDEO_ES, i_chroma );
    p_fmt->i_width          = i_width;
    p_fmt->i_visible_width  = i_visible_width;
    p_fmt->i_height         = i_height;
    p_fmt->i_visible_height = i_visible_height;
    p_fmt->i_x_offset       =
    p_fmt->i_y_offset       = 0;
    vlc_ureduce( &p_fmt->i_sar_num, &p_fmt->i_sar_den,
                 i_sar_num, i_sar_den, 0 );
    switch( p_fmt->i_chroma )
    {
    case VLC_CODEC_YUVA:
        p_fmt->i_bits_per_pixel = 32;
        break;
    case VLC_CODEC_YUV420A:
        p_fmt->i_bits_per_pixel = 20;
        break;
    case VLC_CODEC_YUV422A:
        p_fmt->i_bits_per_pixel = 24;
        break;
    case VLC_CODEC_I444:
    case VLC_CODEC_J444:
        p_fmt->i_bits_per_pixel = 24;
        break;
    case VLC_CODEC_I422:
    case VLC_CODEC_YUYV:
    case VLC_CODEC_YVYU:
    case VLC_CODEC_UYVY:
    case VLC_CODEC_VYUY:
    case VLC_CODEC_J422:
        p_fmt->i_bits_per_pixel = 16;
        break;
    case VLC_CODEC_I440:
    case VLC_CODEC_J440:
        p_fmt->i_bits_per_pixel = 16;
        break;
    case VLC_CODEC_I411:
    case VLC_CODEC_YV12:
    case VLC_CODEC_I420:
    case VLC_CODEC_J420:
        p_fmt->i_bits_per_pixel = 12;
        break;
    case VLC_CODEC_YV9:
    case VLC_CODEC_I410:
        p_fmt->i_bits_per_pixel = 9;
        break;
    case VLC_CODEC_Y211:
        p_fmt->i_bits_per_pixel = 8;
        break;
    case VLC_CODEC_YUVP:
        p_fmt->i_bits_per_pixel = 8;
        break;
    case VLC_CODEC_RGB32:
    case VLC_CODEC_RGBA:
    case VLC_CODEC_ARGB:
    case VLC_CODEC_BGRA:
        p_fmt->i_bits_per_pixel = 32;
        break;
    case VLC_CODEC_RGB24:
        p_fmt->i_bits_per_pixel = 24;
        break;
    case VLC_CODEC_RGB15:
    case VLC_CODEC_RGB16:
        p_fmt->i_bits_per_pixel = 16;
        break;
    case VLC_CODEC_RGB8:
        p_fmt->i_bits_per_pixel = 8;
        break;
    case VLC_CODEC_GREY:
    case VLC_CODEC_RGBP:
        p_fmt->i_bits_per_pixel = 8;
        break;
    case VLC_CODEC_XYZ12:
        p_fmt->i_bits_per_pixel = 48;
        break;
    default:
        p_fmt->i_bits_per_pixel = 0;
        break;
    }
}
