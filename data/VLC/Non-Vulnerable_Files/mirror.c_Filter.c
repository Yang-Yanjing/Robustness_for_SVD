 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    bool b_vertical_split, b_left_to_right;
    int i_index;
    if( !p_pic ) return NULL;
    filter_sys_t *p_sys = p_filter->p_sys;
    b_vertical_split = !atomic_load( &p_sys->i_split );
    b_left_to_right = !atomic_load( &p_sys->i_direction );
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        msg_Warn( p_filter, "can't get output picture" );
        picture_Release( p_pic );
        return NULL;
    }
    for( i_index = 0 ; i_index < p_pic->i_planes ; i_index++ )
    {
        if ( b_vertical_split )
            VerticalMirror( p_pic, p_outpic, i_index, b_left_to_right );
        else
            HorizontalMirror( p_pic, p_outpic, i_index, b_left_to_right );
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
