}
int filter_ConfigureBlend( filter_t *p_blend,
                           int i_dst_width, int i_dst_height,
                           const video_format_t *p_src )
{
    /* */
    if( p_blend->p_module &&
        p_blend->fmt_in.video.i_chroma != p_src->i_chroma )
    {
        /* The chroma is not the same, we need to reload the blend module */
        module_unneed( p_blend, p_blend->p_module );
        p_blend->p_module = NULL;
    }
    /* */
    p_blend->fmt_in.i_codec = p_src->i_chroma;
    p_blend->fmt_in.video   = *p_src;
    /* */
    p_blend->fmt_out.video.i_width          =
    p_blend->fmt_out.video.i_visible_width  = i_dst_width;
    p_blend->fmt_out.video.i_height         =
    p_blend->fmt_out.video.i_visible_height = i_dst_height;
    /* */
    if( !p_blend->p_module )
        p_blend->p_module = module_need( p_blend, "video blending", NULL, false );
    if( !p_blend->p_module )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
