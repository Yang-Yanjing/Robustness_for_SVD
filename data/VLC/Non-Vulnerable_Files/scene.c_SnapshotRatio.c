}
static void SnapshotRatio( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = (filter_sys_t *)p_filter->p_sys;
    if( !p_pic ) return;
    if( p_sys->i_frames % p_sys->i_ratio != 0 )
    {
        p_sys->i_frames++;
        return;
    }
    p_sys->i_frames++;
    if( p_sys->scene.p_pic )
        picture_Release( p_sys->scene.p_pic );
    if( (p_sys->i_width <= 0) && (p_sys->i_height > 0) )
    {
        p_sys->i_width = (p_pic->format.i_width * p_sys->i_height) / p_pic->format.i_height;
    }
    else if( (p_sys->i_height <= 0) && (p_sys->i_width > 0) )
    {
        p_sys->i_height = (p_pic->format.i_height * p_sys->i_width) / p_pic->format.i_width;
    }
    else if( (p_sys->i_width <= 0) && (p_sys->i_height <= 0) )
    {
        p_sys->i_width = p_pic->format.i_width;
        p_sys->i_height = p_pic->format.i_height;
    }
    p_sys->scene.p_pic = picture_NewFromFormat( &p_pic->format );
    if( p_sys->scene.p_pic )
    {
        picture_Copy( p_sys->scene.p_pic, p_pic );
        SavePicture( p_filter, p_sys->scene.p_pic );
    }
}
