 *****************************************************************************/
static void VerticalMirror( picture_t *p_pic, picture_t *p_outpic, int i_plane,
                           bool b_left_to_right )
{
    switch( p_pic->format.i_chroma )
    {
        CASE_PLANAR_YUV_SQUARE
            PlanarVerticalMirror( p_pic, p_outpic, i_plane, b_left_to_right );
            break;
        case VLC_CODEC_YUYV:
        case VLC_CODEC_YVYU:
            YUV422VerticalMirror( p_pic, p_outpic, i_plane, b_left_to_right,
                                 true );
            break;
        case VLC_CODEC_UYVY:
        case VLC_CODEC_CYUV:
            YUV422VerticalMirror( p_pic, p_outpic, i_plane, b_left_to_right,
                                 false );
            break;
        case VLC_CODEC_RGB24:
            RV24VerticalMirror( p_pic, p_outpic, i_plane, b_left_to_right );
            break;
        case VLC_CODEC_RGB32:
            RV32VerticalMirror( p_pic, p_outpic, i_plane, b_left_to_right );
            break;
        default:
            assert( false );
    }
}
