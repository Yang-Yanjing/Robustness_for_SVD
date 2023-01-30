}
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    filter_sys_t *p_sys = p_filter->p_sys;
    int i_plane;
    const int i_dim = p_sys->i_dim;
    type_t *pt_buffer;
    type_t *pt_scale;
    const type_t *pt_distribution = p_sys->pt_distribution;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    if( !p_sys->pt_buffer )
    {
        p_sys->pt_buffer = realloc_or_free( p_sys->pt_buffer,
                               p_pic->p[Y_PLANE].i_visible_lines *
                               p_pic->p[Y_PLANE].i_pitch * sizeof( type_t ) );
    }
    pt_buffer = p_sys->pt_buffer;
    if( !p_sys->pt_scale )
    {
        const int i_visible_lines = p_pic->p[Y_PLANE].i_visible_lines;
        const int i_visible_pitch = p_pic->p[Y_PLANE].i_visible_pitch;
        const int i_pitch = p_pic->p[Y_PLANE].i_pitch;
        int i_col, i_line;
        p_sys->pt_scale = xmalloc( i_visible_lines * i_pitch * sizeof( type_t ) );
        pt_scale = p_sys->pt_scale;
        for( i_line = 0 ; i_line < i_visible_lines ; i_line++ )
        {
            for( i_col = 0; i_col < i_visible_pitch ; i_col++ )
            {
                int x, y;
                type_t t_value = 0;
                for( y = __MAX( -i_dim, -i_line );
                     y <= __MIN( i_dim, i_visible_lines - i_line - 1 );
                     y++ )
                {
                    for( x = __MAX( -i_dim, -i_col );
                         x <= __MIN( i_dim, i_visible_pitch - i_col + 1 );
                         x++ )
                    {
                        t_value += pt_distribution[y+i_dim] *
                                   pt_distribution[x+i_dim];
                    }
                }
                pt_scale[i_line*i_pitch+i_col] = t_value;
            }
        }
    }
    pt_scale = p_sys->pt_scale;
    for( i_plane = 0 ; i_plane < p_pic->i_planes ; i_plane++ )
    {
        uint8_t *p_in = p_pic->p[i_plane].p_pixels;
        uint8_t *p_out = p_outpic->p[i_plane].p_pixels;
        const int i_visible_lines = p_pic->p[i_plane].i_visible_lines;
        const int i_visible_pitch = p_pic->p[i_plane].i_visible_pitch;
        const int i_in_pitch = p_pic->p[i_plane].i_pitch;
        int i_line, i_col;
        const int x_factor = p_pic->p[Y_PLANE].i_visible_pitch/i_visible_pitch-1;
        const int y_factor = p_pic->p[Y_PLANE].i_visible_lines/i_visible_lines-1;
        for( i_line = 0 ; i_line < i_visible_lines ; i_line++ )
        {
            for( i_col = 0; i_col < i_visible_pitch ; i_col++ )
            {
                type_t t_value = 0;
                int x;
                const int c = i_line*i_in_pitch+i_col;
                for( x = __MAX( -i_dim, -i_col*(x_factor+1) );
                     x <= __MIN( i_dim, (i_visible_pitch - i_col)*(x_factor+1) + 1 );
                     x++ )
                {
                    t_value += pt_distribution[x+i_dim] *
                               p_in[c+(x>>x_factor)];
                }
                pt_buffer[c] = t_value;
            }
        }
        for( i_line = 0 ; i_line < i_visible_lines ; i_line++ )
        {
            for( i_col = 0; i_col < i_visible_pitch ; i_col++ )
            {
                type_t t_value = 0;
                int y;
                const int c = i_line*i_in_pitch+i_col;
                for( y = __MAX( -i_dim, (-i_line)*(y_factor+1) );
                     y <= __MIN( i_dim, (i_visible_lines - i_line)*(y_factor+1) - 1 );
                     y++ )
                {
                    t_value += pt_distribution[y+i_dim] *
                               pt_buffer[c+(y>>y_factor)*i_in_pitch];
                }
                const type_t t_scale = pt_scale[(i_line<<y_factor)*(i_in_pitch<<x_factor)+(i_col<<x_factor)];
                p_out[i_line * p_outpic->p[i_plane].i_pitch + i_col] = (uint8_t)(t_value / t_scale); // FIXME wouldn't it be better to round instead of trunc ?
            }
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
