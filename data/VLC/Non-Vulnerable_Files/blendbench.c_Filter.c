 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    filter_t *p_blend;
    if( p_sys->b_done )
        return p_pic;
    p_blend = vlc_object_create( p_filter, sizeof(filter_t) );
    if( !p_blend )
    {
        picture_Release( p_pic );
        return NULL;
    }
    p_blend->fmt_out.video = p_sys->p_base_image->format;
    p_blend->fmt_in.video = p_sys->p_blend_image->format;
    p_blend->p_module = module_need( p_blend, "video blending", NULL, false );
    if( !p_blend->p_module )
    {
        picture_Release( p_pic );
        vlc_object_release( p_blend );
        return NULL;
    }
    mtime_t time = mdate();
    for( int i_iter = 0; i_iter < p_sys->i_loops; ++i_iter )
    {
        p_blend->pf_video_blend( p_blend,
                                 p_sys->p_base_image, p_sys->p_blend_image,
                                 0, 0, p_sys->i_alpha );
    }
    time = mdate() - time;
    msg_Info( p_filter, "Blended %d images in %f sec", p_sys->i_loops,
              time / 1000000.0f );
    msg_Info( p_filter, "Speed is: %f images/second, %f pixels/second",
              (float) p_sys->i_loops / time * 1000000,
              (float) p_sys->i_loops / time * 1000000 *
                  p_sys->p_blend_image->p[Y_PLANE].i_visible_pitch *
                  p_sys->p_blend_image->p[Y_PLANE].i_visible_lines );
    module_unneed( p_blend, p_blend->p_module );
    vlc_object_release( p_blend );
    p_sys->b_done = true;
    return p_pic;
}
