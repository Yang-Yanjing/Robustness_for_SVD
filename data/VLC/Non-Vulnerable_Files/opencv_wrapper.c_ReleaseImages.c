 *****************************************************************************/
static void ReleaseImages( filter_t* p_filter )
{
    filter_sys_t* p_sys = p_filter->p_sys;
    for( int i = 0; i < VOUT_MAX_PLANES; i++ )
    {
        if (p_sys->p_cv_image[i] != NULL)
        {
            cvReleaseImageHeader(&(p_sys->p_cv_image[i]));
            p_sys->p_cv_image[i] = NULL;
        }
    }
    p_sys->i_cv_image_size = 0;
    /* Release temp picture_t if it exists */
    if (p_sys->p_to_be_freed)
    {
        picture_Release( p_sys->p_to_be_freed );
        p_sys->p_to_be_freed = NULL;
    }
#ifndef NDEBUG
    msg_Dbg( p_filter, "images released" );
#endif
}
