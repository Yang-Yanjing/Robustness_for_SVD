 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t* p_filter = (filter_t*)p_this;
    ReleaseImages( p_filter );
    // Release the internal OpenCV filter.
    module_unneed( p_filter->p_sys->p_opencv, p_filter->p_sys->p_opencv->p_module );
    vlc_object_release( p_filter->p_sys->p_opencv );
    p_filter->p_sys->p_opencv = NULL;
    free( p_filter->p_sys );
}
