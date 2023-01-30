 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    vlc_mutex_lock( &p_sys->lock );
    switch( p_pic->format.i_chroma )
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
            switch( p_sys->i_color )
            {
                case RED:
                    get_red_from_yuv420( p_pic, p_outpic,
                                         Y_PLANE, U_PLANE, V_PLANE );
                    break;
                case GREEN:
                    get_green_from_yuv420( p_pic, p_outpic,
                                           Y_PLANE, U_PLANE, V_PLANE );
                    break;
                case BLUE:
                    get_blue_from_yuv420( p_pic, p_outpic,
                                          Y_PLANE, U_PLANE, V_PLANE );
                    break;
                default:
                    get_custom_from_yuv420( p_pic, p_outpic,
                                            Y_PLANE, U_PLANE, V_PLANE,
                                            p_sys->projection_matrix);
                    break;
            }
            break;
        case VLC_CODEC_I422:
        case VLC_CODEC_J422:
            switch( p_filter->p_sys->i_color )
            {
                case RED:
                    get_red_from_yuv422( p_pic, p_outpic,
                                         Y_PLANE, U_PLANE, V_PLANE );
                    break;
                case GREEN:
                    get_green_from_yuv422( p_pic, p_outpic,
                                           Y_PLANE, U_PLANE, V_PLANE );
                    break;
                case BLUE:
                    get_blue_from_yuv422( p_pic, p_outpic,
                                          Y_PLANE, U_PLANE, V_PLANE );
                    break;
                default:
                    get_custom_from_yuv422( p_pic, p_outpic,
                                            Y_PLANE, U_PLANE, V_PLANE,
                                            p_sys->projection_matrix);
                    break;
            }
            break;
        CASE_PACKED_YUV_422
            get_custom_from_packedyuv422( p_pic, p_outpic,
                                          p_sys->projection_matrix );
            break;
        default:
            vlc_mutex_unlock( &p_sys->lock );
            msg_Warn( p_filter, "Unsupported input chroma (%4.4s)",
                      (char*)&(p_pic->format.i_chroma) );
            picture_Release( p_pic );
            return NULL;
    }
    vlc_mutex_unlock( &p_sys->lock );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
