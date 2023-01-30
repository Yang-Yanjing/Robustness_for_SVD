 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
        case VLC_CODEC_I422:
        case VLC_CODEC_J422:
        CASE_PACKED_YUV_422
            break;
        default:
            /* We only want planar YUV 4:2:0 or 4:2:2 */
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                     (char*)&(p_filter->fmt_in.video.i_chroma) );
            return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_filter->p_sys->projection_matrix = malloc( 9 * sizeof( int ) );
    if( !p_filter->p_sys->projection_matrix )
    {
        free( p_filter->p_sys );
        return VLC_ENOMEM;
    }
    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    p_filter->p_sys->i_color = var_CreateGetIntegerCommand( p_filter,
                                               FILTER_PREFIX "component" );
    /* Matrix won't be used for RED, GREEN or BLUE in planar formats */
    make_projection_matrix( p_filter, p_filter->p_sys->i_color,
                            p_filter->p_sys->projection_matrix );
    vlc_mutex_init( &p_filter->p_sys->lock );
    var_AddCallback( p_filter, FILTER_PREFIX "component",
                     ExtractCallback, p_filter->p_sys );
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
