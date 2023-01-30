*****************************************************************************/
static void drawBall( filter_sys_t *p_sys, picture_t *p_outpic )
{
    int x = p_sys->i_ball_x;
    int y = p_sys->i_ball_y;
    int size = p_sys->i_ballSize;
    const int i_width = p_outpic->p[0].i_visible_pitch;
    const int i_height = p_outpic->p[0].i_visible_lines;
    for( int j = y - size; j <= y + size; j++ )
    {
        bool b_skip = ( x - size ) % 2;
        for( int i = x - size; i <= x + size; i++ )
        {
            /* Draw the pixel if it is inside the disk
               and check we don't write out the frame. */
            if( ( i - x ) * ( i - x ) + ( j - y ) * ( j - y ) <= size * size
                && i >= 0 && i < i_width
                && j >= 0 && j < i_height )
            {
                ( *p_sys->drawingPixelFunction )( p_sys, p_outpic,
                                    p_sys->colorList[ p_sys->ballColor ].comp1,
                                    p_sys->colorList[ p_sys->ballColor ].comp2,
                                    p_sys->colorList[ p_sys->ballColor ].comp3,
                                    i, j, b_skip );
            }
            b_skip = !b_skip;
        }
    }
}
