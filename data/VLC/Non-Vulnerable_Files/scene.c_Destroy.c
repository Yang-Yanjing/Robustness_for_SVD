 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = (filter_sys_t *) p_filter->p_sys;
    image_HandlerDelete( p_sys->p_image );
    if( p_sys->scene.p_pic )
        picture_Release( p_sys->scene.p_pic );
    free( p_sys->psz_format );
    free( p_sys->psz_prefix );
    free( p_sys->psz_path );
    free( p_sys );
}
