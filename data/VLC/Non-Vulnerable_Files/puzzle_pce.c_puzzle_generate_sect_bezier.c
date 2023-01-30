 *****************************************************************************/
int puzzle_generate_sect_bezier( filter_t *p_filter, piece_shape_t *ps_piece_shape, uint8_t i_pts_nbr, point_t *ps_pt, uint8_t i_plane, uint8_t i_border)
{
    /* generate data required to draw a sector of puzzle piece using bezier shape */
    if ((!ps_pt) || (!ps_piece_shape))
        return VLC_EGENERIC;
    filter_sys_t *p_sys = p_filter->p_sys;
    int32_t i_width = p_sys->ps_desk_planes[i_plane].i_pce_max_width;
    int32_t i_lines = p_sys->ps_desk_planes[i_plane].i_pce_max_lines;
    int32_t i_size_x_0 = p_sys->ps_desk_planes[0].i_pce_max_width;
    int32_t i_size_y_0 = p_sys->ps_desk_planes[0].i_pce_max_lines;
    float f_x_ratio =  ((float) i_width) / ((float) i_size_x_0);
    float f_y_ratio = ((float) i_lines) / ((float) i_size_y_0);
    /* first: get min x and min y */
    float f_min_curve_x, f_min_curve_y;
    puzzle_get_min_bezier(&f_min_curve_x, &f_min_curve_y, f_x_ratio, f_y_ratio, ps_pt, i_pts_nbr);
    f_min_curve_y = __MIN(0,floor(f_min_curve_y));
    f_min_curve_x = __MIN(0,floor(f_min_curve_x));
    /* next: process each horizontal pixel lines */
    int32_t i_min_y = (i_border==puzzle_SHAPE_TOP)?floor(f_min_curve_y):0;
    int32_t i_nb_y = (i_border==puzzle_SHAPE_TOP)?(i_lines / 2 - i_min_y):i_lines;
    /* allocate memory */
    ps_piece_shape->i_row_nbr = i_nb_y;
    ps_piece_shape->i_first_row_offset = i_min_y;
    ps_piece_shape->ps_piece_shape_row = malloc( sizeof( piece_shape_row_t ) * ps_piece_shape->i_row_nbr );
    if (!ps_piece_shape->ps_piece_shape_row)
        return VLC_ENOMEM;
    return puzzle_generate_shape_lines(p_filter, ps_piece_shape, i_min_y, i_nb_y, f_x_ratio, f_y_ratio, ps_pt, i_pts_nbr, i_border, i_plane);
}
