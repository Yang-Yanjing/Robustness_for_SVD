#ifdef SCREEN_SUBSCREEN
void FollowMouse( demux_sys_t *p_sys, int i_x, int i_y )
{
    i_x -= p_sys->i_width/2;
    if( i_x < 0 ) i_x = 0;
    p_sys->i_left = __MIN( (unsigned int)i_x,
    p_sys->i_screen_width - p_sys->i_width );
    i_y -= p_sys->i_height/2;
    if( i_y < 0 ) i_y = 0;
    p_sys->i_top = __MIN( (unsigned int)i_y,
    p_sys->i_screen_height - p_sys->i_height );
}
