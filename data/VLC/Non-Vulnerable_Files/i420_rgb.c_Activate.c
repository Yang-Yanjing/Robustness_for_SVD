 *****************************************************************************/
static int Activate( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
#ifdef PLAIN
    size_t i_tables_size;
#endif
    if( !vlc_CPU_capable() )
        return VLC_EGENERIC;
    if( p_filter->fmt_out.video.i_width & 1
     || p_filter->fmt_out.video.i_height & 1 )
    {
        return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.orientation != p_filter->fmt_out.video.orientation )
    {
        return VLC_EGENERIC;
    }
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_YV12:
        case VLC_CODEC_I420:
            switch( p_filter->fmt_out.video.i_chroma )
            {
#ifndef PLAIN
                case VLC_CODEC_RGB15:
                case VLC_CODEC_RGB16:
                    /* If we don't have support for the bitmasks, bail out */
                    if( ( p_filter->fmt_out.video.i_rmask == 0x7c00
                       && p_filter->fmt_out.video.i_gmask == 0x03e0
                       && p_filter->fmt_out.video.i_bmask == 0x001f ) )
                    {
                        /* R5G5B6 pixel format */
                        msg_Dbg(p_this, "RGB pixel format is R5G5B5");
                        p_filter->pf_video_filter = I420_R5G5B5_Filter;
                    }
                    else if( ( p_filter->fmt_out.video.i_rmask == 0xf800
                            && p_filter->fmt_out.video.i_gmask == 0x07e0
                            && p_filter->fmt_out.video.i_bmask == 0x001f ) )
                    {
                        /* R5G6B5 pixel format */
                        msg_Dbg(p_this, "RGB pixel format is R5G6B5");
                        p_filter->pf_video_filter = I420_R5G6B5_Filter;
                    }
                    else
                        return VLC_EGENERIC;
                    break;
                case VLC_CODEC_RGB32:
                    /* If we don't have support for the bitmasks, bail out */
                    if( p_filter->fmt_out.video.i_rmask == 0x00ff0000
                     && p_filter->fmt_out.video.i_gmask == 0x0000ff00
                     && p_filter->fmt_out.video.i_bmask == 0x000000ff )
                    {
                        /* A8R8G8B8 pixel format */
                        msg_Dbg(p_this, "RGB pixel format is A8R8G8B8");
                        p_filter->pf_video_filter = I420_A8R8G8B8_Filter;
                    }
                    else if( p_filter->fmt_out.video.i_rmask == 0xff000000
                          && p_filter->fmt_out.video.i_gmask == 0x00ff0000
                          && p_filter->fmt_out.video.i_bmask == 0x0000ff00 )
                    {
                        /* R8G8B8A8 pixel format */
                        msg_Dbg(p_this, "RGB pixel format is R8G8B8A8");
                        p_filter->pf_video_filter = I420_R8G8B8A8_Filter;
                    }
                    else if( p_filter->fmt_out.video.i_rmask == 0x0000ff00
                          && p_filter->fmt_out.video.i_gmask == 0x00ff0000
                          && p_filter->fmt_out.video.i_bmask == 0xff000000 )
                    {
                        /* B8G8R8A8 pixel format */
                        msg_Dbg(p_this, "RGB pixel format is B8G8R8A8");
                        p_filter->pf_video_filter = I420_B8G8R8A8_Filter;
                    }
                    else if( p_filter->fmt_out.video.i_rmask == 0x000000ff
                          && p_filter->fmt_out.video.i_gmask == 0x0000ff00
                          && p_filter->fmt_out.video.i_bmask == 0x00ff0000 )
                    {
                        /* A8B8G8R8 pixel format */
                        msg_Dbg(p_this, "RGB pixel format is A8B8G8R8");
                        p_filter->pf_video_filter = I420_A8B8G8R8_Filter;
                    }
                    else
                        return VLC_EGENERIC;
                    break;
#else
                case VLC_CODEC_RGB8:
                    p_filter->pf_video_filter = I420_RGB8_Filter;
                    break;
                case VLC_CODEC_RGB15:
                case VLC_CODEC_RGB16:
                    p_filter->pf_video_filter = I420_RGB16_Filter;
                    break;
                case VLC_CODEC_RGB32:
                    p_filter->pf_video_filter = I420_RGB32_Filter;
                    break;
#endif
                default:
                    return VLC_EGENERIC;
            }
            break;
        default:
            return VLC_EGENERIC;
    }
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
    {
        return VLC_EGENERIC;
    }
    switch( p_filter->fmt_out.video.i_chroma )
    {
#ifdef PLAIN
        case VLC_CODEC_RGB8:
            p_filter->p_sys->p_buffer = malloc( VOUT_MAX_WIDTH );
            break;
#endif
        case VLC_CODEC_RGB15:
        case VLC_CODEC_RGB16:
            p_filter->p_sys->p_buffer = malloc( VOUT_MAX_WIDTH * 2 );
            break;
        case VLC_CODEC_RGB24:
        case VLC_CODEC_RGB32:
            p_filter->p_sys->p_buffer = malloc( VOUT_MAX_WIDTH * 4 );
            break;
        default:
            p_filter->p_sys->p_buffer = NULL;
            break;
    }
    if( p_filter->p_sys->p_buffer == NULL )
    {
        free( p_filter->p_sys );
        return VLC_EGENERIC;
    }
    p_filter->p_sys->p_offset = malloc( p_filter->fmt_out.video.i_width
                    * ( ( p_filter->fmt_out.video.i_chroma
                           == VLC_CODEC_RGB8 ) ? 2 : 1 )
                    * sizeof( int ) );
    if( p_filter->p_sys->p_offset == NULL )
    {
        free( p_filter->p_sys->p_buffer );
        free( p_filter->p_sys );
        return VLC_EGENERIC;
    }
#ifdef PLAIN
    switch( p_filter->fmt_out.video.i_chroma )
    {
    case VLC_CODEC_RGB8:
        i_tables_size = sizeof( uint8_t ) * PALETTE_TABLE_SIZE;
        break;
    case VLC_CODEC_RGB15:
    case VLC_CODEC_RGB16:
        i_tables_size = sizeof( uint16_t ) * RGB_TABLE_SIZE;
        break;
    default: /* RV24, RV32 */
        i_tables_size = sizeof( uint32_t ) * RGB_TABLE_SIZE;
        break;
    }
    p_filter->p_sys->p_base = malloc( i_tables_size );
    if( p_filter->p_sys->p_base == NULL )
    {
        free( p_filter->p_sys->p_offset );
        free( p_filter->p_sys->p_buffer );
        free( p_filter->p_sys );
        return -1;
    }
    SetYUV( p_filter );
#endif
    return 0;
}
