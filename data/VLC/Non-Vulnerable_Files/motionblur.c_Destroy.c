 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    var_DelCallback( p_filter, FILTER_PREFIX "factor",
                     MotionBlurCallback, p_filter->p_sys );
    picture_Release( p_filter->p_sys->p_tmp );
    free( p_filter->p_sys );
}
