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
    switch( p_filter->p_sys->i_mode )
    {
        case EDGE:
            FilterEdge( p_filter, p_pic, p_outpic );
            break;
        case GRADIENT:
            FilterGradient( p_filter, p_pic, p_outpic );
            break;
        case HOUGH:
            FilterHough( p_filter, p_pic, p_outpic );
            break;
        default:
            break;
    }
    vlc_mutex_unlock( &p_filter->p_sys->lock );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
