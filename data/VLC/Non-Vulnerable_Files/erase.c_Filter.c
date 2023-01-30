 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* If the mask is empty: just copy the image */
    vlc_mutex_lock( &p_sys->lock );
    if( p_sys->p_mask )
        FilterErase( p_filter, p_pic, p_outpic );
    else
        picture_CopyPixels( p_outpic, p_pic );
    vlc_mutex_unlock( &p_sys->lock );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
