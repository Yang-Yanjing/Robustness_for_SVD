 *****************************************************************************/
static void FilterBall( filter_t *p_filter, picture_t *p_inpic,
                           picture_t *p_outpic )
{
    int x, y;
    filter_sys_t *p_sys = p_filter->p_sys;
    uint32_t *p_smooth;
    int32_t *p_grad_x;
    int32_t *p_grad_y;
    picture_t *p_converted;
    video_format_t fmt_comp;
    memset( &fmt_comp, 0, sizeof(fmt_comp) );
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_RGB24:
        CASE_PACKED_YUV_422
            fmt_comp.i_width = p_filter->fmt_in.video.i_width;
            fmt_comp.i_height = p_filter->fmt_in.video.i_height;
            fmt_comp.i_chroma = VLC_FOURCC('G','R','E','Y');
            fmt_comp.i_visible_width = fmt_comp.i_width;
            fmt_comp.i_visible_height = fmt_comp.i_height;
            p_converted = image_Convert( p_filter->p_sys->p_image, p_inpic,
                                         &(p_filter->fmt_in.video),
                                         &fmt_comp );
            if( !p_converted )
                return;
            break;
        default:
            p_converted = p_inpic;
            break;
    }
    const int i_numCols = p_converted->p[0].i_visible_pitch;
    const int i_numLines = p_converted->p[0].i_visible_lines;
    if( !p_filter->p_sys->p_smooth )
        p_filter->p_sys->p_smooth =
                (uint32_t *)malloc( i_numLines * i_numCols
                                    * sizeof(uint32_t));
    p_smooth = p_filter->p_sys->p_smooth;
    if( !p_filter->p_sys->p_grad_x )
        p_filter->p_sys->p_grad_x =
                (int32_t *)malloc( i_numLines * i_numCols
                                   * sizeof(int32_t));
    p_grad_x = p_filter->p_sys->p_grad_x;
    if( !p_filter->p_sys->p_grad_y )
        p_filter->p_sys->p_grad_y =
                (int32_t *)malloc( i_numLines * i_numCols
                                   * sizeof(int32_t));
    p_grad_y = p_filter->p_sys->p_grad_y;
    if( !p_smooth || !p_grad_x || !p_grad_y ) return;
    memcpy( p_outpic->p[0].p_pixels, p_inpic->p[0].p_pixels,
                p_outpic->p[0].i_lines * p_outpic->p[0].i_pitch );
    memcpy( p_outpic->p[1].p_pixels, p_inpic->p[1].p_pixels,
                p_outpic->p[1].i_lines * p_outpic->p[1].i_pitch );
    memcpy( p_outpic->p[2].p_pixels, p_inpic->p[2].p_pixels,
                p_outpic->p[2].i_lines * p_outpic->p[2].i_pitch );
    GaussianConvolution( p_converted, p_smooth );
    /* Compute the picture Sobel gradient
       | -1 0 1 |     |  1  2  1 |
       | -2 0 2 | and |  0  0  0 |
       | -1 0 1 |     | -1 -2 -1 | */
    for( y = 1; y < i_numLines - 1; y++ )
    {
        for( x = 1; x < i_numCols - 1; x++ )
        {
            p_grad_x[ y * i_numCols + x ] =
                    ( p_smooth[(y-1)*i_numCols+x-1]
                    - p_smooth[(y+1)*i_numCols+x-1] )
                    + ( ( p_smooth[(y-1)*i_numCols+x]
                    - p_smooth[(y+1)*i_numCols+x] ) <<1 )
                    + ( p_smooth[(y-1)*i_numCols+x+1]
                    - p_smooth[(y+1)*i_numCols+x+1] );
            p_grad_y[ y * i_numCols + x ] =
                    ( p_smooth[(y-1)*i_numCols+x-1]
                    - p_smooth[(y-1)*i_numCols+x+1] )
                    + ( ( p_smooth[y*i_numCols+x-1]
                    - p_smooth[y*i_numCols+x+1] ) <<1 )
                    + ( p_smooth[(y+1)*i_numCols+x-1]
                    - p_smooth[(y+1)*i_numCols+x+1] );
        }
    }
    if( p_sys->b_edgeVisible )
    {
        /* Display the edges. */
        for( y = 1; y < i_numLines - 1; y++ )
        {
            for( x = 1; x < i_numCols - 1; x++ )
            {
                if( abs( p_grad_x[ y * i_numCols + x ] )
                    + abs( p_grad_y[ y * i_numCols + x ] )
                    > p_sys->i_gradThresh )
                {
                    ( *p_sys->drawingPixelFunction )( p_sys, p_outpic,
                                                      p_filter->p_sys->colorList[ WHITE ].comp1,
                                                      p_filter->p_sys->colorList[ WHITE ].comp2,
                                                      p_filter->p_sys->colorList[ WHITE ].comp3,
                                                      x, y, 0 );
                }
            }
        }
    }
    int i_motion;
    float *pf_lastVect_x = &p_sys->f_lastVect_x;
    float *pf_lastVect_y = &p_sys->f_lastVect_y;
    float f_newVect_x = 0;
    float f_newVect_y = 0;
    float f_contVect_x = 0;
    float f_contVect_y = 0;
    int nb_collisions = 0;
    bool bounce = false;
    /* Test collisions for each pixel the ball will cover in its
       motion. */
    for ( i_motion = 0; i_motion <= p_sys->i_ballSpeed && !bounce; i_motion++ )
    {
        /* Compute next ball position */
        x = roundf( (float)p_sys->i_ball_x
                    + *pf_lastVect_x * (float)i_motion );
        y = roundf( (float)p_sys->i_ball_y
                    + *pf_lastVect_y * (float)i_motion );
        for( int i = x - p_sys->i_ballSize; i <= x + p_sys->i_ballSize; i++ )
        {
            for( int j = y - p_sys->i_ballSize;
                 j <= y + p_sys->i_ballSize; j++ )
            {
                /* Test the pixel if it is inside the disk and check we don't
                write out the frame. */
                if( ( i - x ) * ( i - x ) + ( j - y ) * ( j - y )
                    == p_sys->i_ballSize * p_sys->i_ballSize
                    && j <= i_numLines - 1 && x <= i_numCols - 1
                    && j >= 0 && i >= 0 )
                {
                    /* Test firstly the picture limit collisions. */
                    if( i <= 2 )
                    {
                        f_contVect_x = x - i;
                        f_contVect_y = 0;
                        x++;
                        bounce = true;
                        nb_collisions = 1;
                        goto endLoop;
                    }
                    if( j <= 2 )
                    {
                        f_contVect_x = 0;
                        f_contVect_y = y - j;
                        y++;
                        bounce = true;
                        nb_collisions = 1;
                        goto endLoop;
                    }
                    if( j >= i_numLines - 3 )
                    {
                        f_contVect_x = 0;
                        f_contVect_y = y - j;
                        y--;
                        bounce = true;
                        nb_collisions = 1;
                        goto endLoop;
                    }
                    if( i >= i_numCols - 3 )
                    {
                        f_contVect_x = x - i;
                        f_contVect_y = 0;
                        x--;
                        bounce = true;
                        nb_collisions = 1;
                        goto endLoop;
                    }
                    /* Test the collisions with edges. */
                    if( abs( p_grad_x[ j * i_numCols + i ] )
                        + abs( p_grad_y[ j * i_numCols + i ] )
                        > p_sys->i_gradThresh )
                    {
                        f_contVect_x += x - i;
                        f_contVect_y += y - j;
                        nb_collisions++;
                        bounce = true;
                    }
                }
            }
        }
    }
    endLoop:
    if( bounce )
    {
        /* Compute normal vector. */
        f_contVect_x /= nb_collisions;
        f_contVect_y /= nb_collisions;
        NormalizeVector( &f_contVect_x, &f_contVect_y );
        /* Compute the new vector after the bounce. */
        float cosinus = *pf_lastVect_x * f_contVect_x
                        + *pf_lastVect_y * f_contVect_y;
        f_newVect_x = *pf_lastVect_x - 2 * cosinus * f_contVect_x;
        f_newVect_y = *pf_lastVect_y - 2 * cosinus * f_contVect_y;
        NormalizeVector( &f_newVect_x, &f_newVect_y );
        *pf_lastVect_x = f_newVect_x;
        *pf_lastVect_y = f_newVect_y;
        p_sys->i_ball_x = x;
        p_sys->i_ball_y = y;
        /* Test if next pixel is outside the frame limits.
           If it is the case, then the ball is blocked until it can move. */
        x = roundf( (float)x + *pf_lastVect_x );
        y = roundf( (float)y + *pf_lastVect_y );
        if( x - p_sys->i_ballSize < 2
            || x + p_sys->i_ballSize > i_numCols - 3
            || y - p_sys->i_ballSize < 2
            || y + p_sys->i_ballSize > i_numLines - 3 )
        {
            *pf_lastVect_x = 0;
            *pf_lastVect_y = 0;
        }
        else
            /* After a bouce, the first ball motion is always one pixel. */
            i_motion = p_sys->i_ballSpeed - 1;
    }
    else
        i_motion = 0;
    /* Compute next ball position. */
    p_sys->i_ball_x = roundf( (float)p_sys->i_ball_x + *pf_lastVect_x
                              * (float)( p_sys->i_ballSpeed - i_motion ) );
    p_sys->i_ball_y = roundf( p_sys->i_ball_y + *pf_lastVect_y
                              * (float)( p_sys->i_ballSpeed - i_motion ) );
    /* Draw the ball */
    drawBall( p_sys, p_outpic );
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_RGB24:
        CASE_PACKED_YUV_422
            picture_Release( p_converted );
        default:
            break;
    }
}
