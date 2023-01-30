 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    if( p_filter->p_sys->p_image )
        image_HandlerDelete( p_filter->p_sys->p_image );
    p_filter->p_sys->p_image = NULL;
    free( p_filter->p_sys );
}
