 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_Release( p_sys->p_base_image );
    picture_Release( p_sys->p_blend_image );
}
