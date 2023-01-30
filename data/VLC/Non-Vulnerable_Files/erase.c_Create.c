 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_filename;
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
        case VLC_CODEC_I422:
        case VLC_CODEC_J422:
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                     (char*)&(p_filter->fmt_in.video.i_chroma) );
            return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys = p_filter->p_sys;
    p_filter->pf_video_filter = Filter;
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    psz_filename =
        var_CreateGetNonEmptyStringCommand( p_filter, CFG_PREFIX "mask" );
    if( !psz_filename )
    {
        msg_Err( p_filter, "Missing 'mask' option value." );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_sys->p_mask = NULL;
    LoadMask( p_filter, psz_filename );
    free( psz_filename );
    p_sys->i_x = var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "x" );
    p_sys->i_y = var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "y" );
    vlc_mutex_init( &p_sys->lock );
    var_AddCallback( p_filter, CFG_PREFIX "x", EraseCallback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "y", EraseCallback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "mask", EraseCallback, p_sys );
    return VLC_SUCCESS;
}
