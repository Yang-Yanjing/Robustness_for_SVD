}
static void Draw( filter_t *p_filter, uint8_t *p_pix, int i_pix_pitch, int i_pix_size )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    int i, j;
    for( i = 1, j = 0; i < p_sys->i_colors; i++ )
    {
        int x, y;
        if( p_sys->colors[i] != i )
            continue;
        const int color_x_min = p_sys->color_x_min[i];
        const int color_x_max = p_sys->color_x_max[i];
        const int color_y_min = p_sys->color_y_min[i];
        const int color_y_max = p_sys->color_y_max[i];
        if( color_x_min == -1 )
            continue;
        if( ( color_y_max - color_y_min ) * ( color_x_max - color_x_min ) < 16 )
            continue;
        j++;
        y = color_y_min;
        for( x = color_x_min; x <= color_x_max; x++ )
            p_pix[y*i_pix_pitch+x*i_pix_size] = 0xff;
        y = color_y_max;
        for( x = color_x_min; x <= color_x_max; x++ )
            p_pix[y*i_pix_pitch+x*i_pix_size] = 0xff;
        x = color_x_min;
        for( y = color_y_min; y <= color_y_max; y++ )
            p_pix[y*i_pix_pitch+x*i_pix_size] = 0xff;
        x = color_x_max;
        for( y = color_y_min; y <= color_y_max; y++ )
            p_pix[y*i_pix_pitch+x*i_pix_size] = 0xff;
    }
    msg_Dbg( p_filter, "Counted %d moving shapes.", j );
}
