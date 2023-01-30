 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    switch( p_filter->fmt_in.video.i_chroma )
    {
        CASE_PLANAR_YUV_SQUARE
            break;
        CASE_PACKED_YUV_422
            break;
        case VLC_CODEC_RGB24:
        case VLC_CODEC_RGB32:
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                     (char*)&(p_filter->fmt_in.video.i_chroma) );
            return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        msg_Err( p_filter, "Input and output chromas don't match" );
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_sys = p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    atomic_init( &p_sys->i_split,
                 var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "split" ) );
    atomic_init( &p_sys->i_direction,
                 var_CreateGetIntegerCommand( p_filter,
                                                    CFG_PREFIX "direction" ) );
    var_AddCallback( p_filter, CFG_PREFIX "split", FilterCallback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "direction", FilterCallback, p_sys );
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
