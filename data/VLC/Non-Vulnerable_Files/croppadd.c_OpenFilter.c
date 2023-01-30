 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys;
    if( !p_filter->b_allow_fmt_out_change )
    {
        msg_Err( p_filter, "Picture format change isn't allowed" );
        return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        msg_Err( p_filter, "Input and output chromas don't match" );
        /* In fact we don't really care about this since we're allowed
         * to change the output format ... FIXME? */
        return VLC_EGENERIC;
    }
    const vlc_chroma_description_t *p_chroma =
        vlc_fourcc_GetChromaDescription( p_filter->fmt_in.video.i_chroma );
    if( p_chroma == NULL || p_chroma->plane_count == 0 )
        return VLC_EGENERIC;
    p_filter->p_sys = (filter_sys_t *)malloc( sizeof( filter_sys_t ) );
    if( !p_filter->p_sys ) return VLC_ENOMEM;
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    p_sys = p_filter->p_sys;
#define GET_OPTION( name ) \
    p_sys->i_ ## name = var_CreateGetInteger( p_filter, CFG_PREFIX #name ); \
    if( p_sys->i_ ## name & 1 ) \
        msg_Warn( p_filter, "Using even values for `" #name "' is recommended" );
    GET_OPTION( croptop )
    GET_OPTION( cropbottom )
    GET_OPTION( cropleft )
    GET_OPTION( cropright )
    GET_OPTION( paddtop )
    GET_OPTION( paddbottom )
    GET_OPTION( paddleft )
    GET_OPTION( paddright )
    p_filter->fmt_out.video.i_height =
    p_filter->fmt_out.video.i_visible_height =
        p_filter->fmt_in.video.i_visible_height
        - p_sys->i_croptop - p_sys->i_cropbottom
        + p_sys->i_paddtop + p_sys->i_paddbottom;
    p_filter->fmt_out.video.i_width =
    p_filter->fmt_out.video.i_visible_width =
        p_filter->fmt_in.video.i_visible_width
        - p_sys->i_cropleft - p_sys->i_cropright
        + p_sys->i_paddleft + p_sys->i_paddright;
    p_filter->pf_video_filter = Filter;
    msg_Dbg( p_filter, "Crop: Top: %d, Bottom: %d, Left: %d, Right: %d",
             p_sys->i_croptop, p_sys->i_cropbottom, p_sys->i_cropleft,
             p_sys->i_cropright );
    msg_Dbg( p_filter, "Padd: Top: %d, Bottom: %d, Left: %d, Right: %d",
             p_sys->i_paddtop, p_sys->i_paddbottom, p_sys->i_paddleft,
             p_sys->i_paddright );
    msg_Dbg( p_filter, "%dx%d -> %dx%d",
             p_filter->fmt_in.video.i_width,
             p_filter->fmt_in.video.i_height,
             p_filter->fmt_out.video.i_width,
             p_filter->fmt_out.video.i_height );
    return VLC_SUCCESS;
}
