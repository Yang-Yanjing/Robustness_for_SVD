 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    const vlc_chroma_description_t *p_chroma =
        vlc_fourcc_GetChromaDescription( p_filter->fmt_in.video.i_chroma );
    if( p_chroma == NULL || p_chroma->plane_count == 0 )
        return VLC_EGENERIC;
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_vfilter_options,
                       p_filter->p_cfg );
    p_filter->p_sys = p_sys = calloc( 1, sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->p_image = image_HandlerCreate( p_this );
    if( !p_sys->p_image )
    {
        msg_Err( p_this, "Couldn't get handle to image conversion routines." );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_sys->psz_format = var_CreateGetString( p_this, CFG_PREFIX "format" );
    p_sys->i_format = image_Type2Fourcc( p_sys->psz_format );
    if( !p_sys->i_format )
    {
        msg_Err( p_filter, "Could not find FOURCC for image type '%s'",
                 p_sys->psz_format );
        image_HandlerDelete( p_sys->p_image );
        free( p_sys->psz_format );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_sys->i_width = var_CreateGetInteger( p_this, CFG_PREFIX "width" );
    p_sys->i_height = var_CreateGetInteger( p_this, CFG_PREFIX "height" );
    p_sys->i_ratio = var_CreateGetInteger( p_this, CFG_PREFIX "ratio" );
    if( p_sys->i_ratio <= 0)
        p_sys->i_ratio = 1;
    p_sys->b_replace = var_CreateGetBool( p_this, CFG_PREFIX "replace" );
    p_sys->psz_prefix = var_CreateGetString( p_this, CFG_PREFIX "prefix" );
    p_sys->psz_path = var_GetNonEmptyString( p_this, CFG_PREFIX "path" );
    if( p_sys->psz_path == NULL )
        p_sys->psz_path = config_GetUserDir( VLC_PICTURES_DIR );
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
