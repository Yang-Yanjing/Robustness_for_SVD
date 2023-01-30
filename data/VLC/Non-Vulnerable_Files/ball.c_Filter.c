 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    vlc_mutex_lock( &p_filter->p_sys->lock );
    FilterBall( p_filter, p_pic, p_outpic );
    vlc_mutex_unlock( &p_filter->p_sys->lock );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
