/* */
void video_format_FixRgb( video_format_t *p_fmt )
{
    /* FIXME find right default mask */
    if( !p_fmt->i_rmask || !p_fmt->i_gmask || !p_fmt->i_bmask )
    {
        switch( p_fmt->i_chroma )
        {
        case VLC_CODEC_RGB15:
            p_fmt->i_rmask = 0x7c00;
            p_fmt->i_gmask = 0x03e0;
            p_fmt->i_bmask = 0x001f;
            break;
        case VLC_CODEC_RGB16:
            p_fmt->i_rmask = 0xf800;
            p_fmt->i_gmask = 0x07e0;
            p_fmt->i_bmask = 0x001f;
            break;
        case VLC_CODEC_RGB24:
            p_fmt->i_rmask = 0xff0000;
            p_fmt->i_gmask = 0x00ff00;
            p_fmt->i_bmask = 0x0000ff;
            break;
        case VLC_CODEC_RGB32:
            p_fmt->i_rmask = 0x00ff0000;
            p_fmt->i_gmask = 0x0000ff00;
            p_fmt->i_bmask = 0x000000ff;
            break;
        default:
            return;
        }
    }
    MaskToShift( &p_fmt->i_lrshift, &p_fmt->i_rrshift,
                 p_fmt->i_rmask );
    MaskToShift( &p_fmt->i_lgshift, &p_fmt->i_rgshift,
                 p_fmt->i_gmask );
    MaskToShift( &p_fmt->i_lbshift, &p_fmt->i_rbshift,
                 p_fmt->i_bmask );
}
