 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t * p_outpic;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    if( p_sys->b_first )
    {
        picture_CopyPixels( p_sys->p_tmp, p_pic );
        p_sys->b_first = false;
    }
    /* Get a new picture */
    RenderBlur( p_sys, p_pic, p_outpic );
    picture_CopyPixels( p_sys->p_tmp, p_outpic );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
