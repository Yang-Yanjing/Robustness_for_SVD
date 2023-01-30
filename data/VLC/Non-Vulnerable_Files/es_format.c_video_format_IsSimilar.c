}
bool video_format_IsSimilar( const video_format_t *p_fmt1, const video_format_t *p_fmt2 )
{
    video_format_t v1 = *p_fmt1;
    video_format_t v2 = *p_fmt2;
    if( v1.i_chroma != v2.i_chroma )
        return false;
    if( v1.i_width != v2.i_width || v1.i_height != v2.i_height ||
        v1.i_visible_width != v2.i_visible_width ||
        v1.i_visible_height != v2.i_visible_height ||
        v1.i_x_offset != v2.i_x_offset || v1.i_y_offset != v2.i_y_offset )
        return false;
    if( v1.i_sar_num * v2.i_sar_den != v2.i_sar_num * v1.i_sar_den )
        return false;
    if( v1.orientation != v2.orientation)
        return false;
    if( v1.i_chroma == VLC_CODEC_RGB15 ||
        v1.i_chroma == VLC_CODEC_RGB16 ||
        v1.i_chroma == VLC_CODEC_RGB24 ||
        v1.i_chroma == VLC_CODEC_RGB32 )
    {
        video_format_FixRgb( &v1 );
        video_format_FixRgb( &v2 );
        if( v1.i_rmask != v2.i_rmask ||
            v1.i_gmask != v2.i_gmask ||
            v1.i_bmask != v2.i_bmask )
            return false;
    }
    return true;
}
