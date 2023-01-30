 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    if( !p_pic ) return NULL;
    filter_sys_t *p_sys = p_filter->p_sys;
    int intensity = atomic_load( &p_sys->i_intensity );
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        msg_Warn( p_filter, "can't get output picture" );
        picture_Release( p_pic );
        return NULL;
    }
    p_sys->pf_sepia( p_pic, p_outpic, intensity );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
