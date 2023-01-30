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
    if( p_sys->p_motion != NULL )
    {
        int i_angle = motion_get_angle( p_sys->p_motion );
        store_trigo( p_sys, i_angle / 20.f );
    }
    int i_sin, i_cos;
    fetch_trigo( p_sys, &i_sin, &i_cos );
    for( int i_plane = 0 ; i_plane < p_pic->i_planes ; i_plane++ )
    {
        plane_t *p_srcp = &p_pic->p[i_plane];
        plane_t *p_dstp = &p_outpic->p[i_plane];
        const int i_visible_lines = p_srcp->i_visible_lines;
        const int i_visible_pitch = p_srcp->i_visible_pitch;
        const int i_aspect = __MAX( 1, ( i_visible_lines * p_pic->p[Y_PLANE].i_visible_pitch ) / ( p_pic->p[Y_PLANE].i_visible_lines * i_visible_pitch ));
        /* = 2 for U and V planes in YUV 4:2:2, = 1 otherwise */
        const int i_line_center = i_visible_lines>>1;
        const int i_col_center  = i_visible_pitch>>1;
        const uint8_t black_pixel = ( i_plane == Y_PLANE ) ? 0x00 : 0x80;
        const int i_line_next =  i_cos / i_aspect -i_sin*i_visible_pitch;
        const int i_col_next  = -i_sin / i_aspect -i_cos*i_visible_pitch;
        int i_line_orig0 = ( - i_cos * i_line_center / i_aspect
                             - i_sin * i_col_center + (1<<11) );
        int i_col_orig0 =    i_sin * i_line_center / i_aspect
                           - i_cos * i_col_center + (1<<11);
        for( int y = 0; y < i_visible_lines; y++)
        {
            uint8_t *p_out = &p_dstp->p_pixels[y * p_dstp->i_pitch];
            for( int x = 0; x < i_visible_pitch; x++, p_out++ )
            {
                const int i_line_orig = (i_line_orig0>>12)*i_aspect + i_line_center;
                const int i_col_orig  = (i_col_orig0>>12)  + i_col_center;
                const uint8_t *p_orig_offset = &p_srcp->p_pixels[i_line_orig * p_srcp->i_pitch + i_col_orig];
                const uint8_t i_line_percent = (i_line_orig0>>4) & 255;
                const uint8_t i_col_percent  = (i_col_orig0 >>4) & 255;
                if(    -1 <= i_line_orig && i_line_orig < i_visible_lines
                    && -1 <= i_col_orig  && i_col_orig  < i_visible_pitch )
                {
                #define test 1
                #undef test
                #ifdef test
                    if( ( i_col_orig > i_visible_pitch/2 ) )
                #endif
                    {
                        uint8_t i_curpix = black_pixel;
                        uint8_t i_colpix = black_pixel;
                        uint8_t i_linpix = black_pixel;
                        uint8_t i_nexpix = black_pixel;
                        if( ( 0 <= i_line_orig ) && ( 0 <= i_col_orig ) )
                            i_curpix = *p_orig_offset;
                        p_orig_offset++;
                        if(  ( i_col_orig < i_visible_pitch - 1)
                             && ( i_line_orig >= 0 ) )
                            i_colpix = *p_orig_offset;
                        p_orig_offset += p_srcp->i_pitch;
                        if( ( i_line_orig < i_visible_lines - 1)
                            && ( i_col_orig  < i_visible_pitch - 1) )
                            i_nexpix = *p_orig_offset;
                        p_orig_offset--;
                        if(  ( i_line_orig < i_visible_lines - 1)
                             && ( i_col_orig >= 0 ) )
                            i_linpix = *p_orig_offset;
                        unsigned int temp = 0;
                        temp+= i_curpix *
                            (256 - i_line_percent) * ( 256 - i_col_percent );
                        temp+= i_linpix *
                            i_line_percent * (256 - i_col_percent );
                        temp+= i_nexpix *
                            ( i_col_percent) * ( i_line_percent);
                        temp+= i_colpix *
                            i_col_percent * (256 - i_line_percent );
                        *p_out = temp >> 16;
                    }
                #ifdef test
                    else if (i_col_orig == i_visible_pitch/2 )
                    {   *p_out = black_pixel;
                    }
                    else
                        *p_out = *p_orig_offset;
                #endif
                #undef test
                }
                else
                {
                    *p_out = black_pixel;
                }
                i_line_orig0 += i_sin;
                i_col_orig0 += i_cos;
            }
            i_line_orig0 += i_line_next;
            i_col_orig0 += i_col_next;
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
