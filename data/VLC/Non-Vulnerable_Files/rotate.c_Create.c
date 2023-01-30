 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        msg_Err( p_filter, "Input and output chromas don't match" );
        return VLC_EGENERIC;
    }
    switch( p_filter->fmt_in.video.i_chroma )
    {
        CASE_PLANAR_YUV
            p_filter->pf_video_filter = Filter;
            break;
        CASE_PACKED_YUV_422
            p_filter->pf_video_filter = FilterPacked;
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
    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    if( var_InheritBool( p_filter, FILTER_PREFIX "use-motion" ) )
    {
        p_sys->p_motion = motion_create( VLC_OBJECT( p_filter ) );
        if( p_sys->p_motion == NULL )
        {
            free( p_sys );
            return VLC_EGENERIC;
        }
    }
    else
    {
        float f_angle = var_CreateGetFloatCommand( p_filter,
                                                   FILTER_PREFIX "angle" );
        store_trigo( p_sys, f_angle );
        var_AddCallback( p_filter, FILTER_PREFIX "angle",
                         RotateCallback, p_sys );
        p_sys->p_motion = NULL;
    }
    return VLC_SUCCESS;
}
