 *****************************************************************************/
int puzzle_generate_sectLeft2Right( filter_t *p_filter, piece_shape_t *ps_piece_shape, piece_shape_t *ps_left_piece_shape, uint8_t i_plane)
{
    if ((!ps_piece_shape) || (!ps_left_piece_shape))
        return VLC_EGENERIC;
    filter_sys_t *p_sys = p_filter->p_sys;
    int32_t i_min_y = ps_left_piece_shape->i_first_row_offset;
    int32_t i_nb_y = ps_left_piece_shape->i_row_nbr;
    /* allocate memory */
    ps_piece_shape->i_row_nbr = i_nb_y;
    ps_piece_shape->i_first_row_offset = i_min_y;
    ps_piece_shape->ps_piece_shape_row = malloc( sizeof( piece_shape_row_t ) * i_nb_y );
    if (!ps_piece_shape->ps_piece_shape_row)
        return VLC_ENOMEM;
    for (int32_t i_y = i_min_y; i_y < i_nb_y + i_min_y; i_y++) {
        int32_t i_row = i_y - i_min_y;
        int32_t i_width = p_sys->ps_desk_planes[i_plane].i_pce_max_width;
        int32_t i_left_width = puzzle_diagonal_limit( p_filter, i_y, true, i_plane  );
        int32_t i_right_width = i_width - puzzle_diagonal_limit( p_filter, i_y, false, i_plane );
        int16_t i_section_nbr = ps_left_piece_shape->ps_piece_shape_row[i_row].i_section_nbr;
        ps_piece_shape->ps_piece_shape_row[i_row].i_section_nbr = i_section_nbr;
        ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section = malloc (  sizeof(row_section_t) * i_section_nbr);
        if (!ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section) {
            for (uint8_t i=0; i<i_row;i++)
                free(ps_piece_shape->ps_piece_shape_row[i].ps_row_section);
            free(ps_piece_shape->ps_piece_shape_row);
            ps_piece_shape->ps_piece_shape_row = NULL;
            return VLC_ENOMEM;
        }
        ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[0].i_type =
                ps_left_piece_shape->ps_piece_shape_row[i_row].ps_row_section[0].i_type;
        ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[0].i_width =
                ps_left_piece_shape->ps_piece_shape_row[i_row].ps_row_section[0].i_width + i_right_width - i_left_width;
        for (int8_t i_s=0; i_s<i_section_nbr;i_s++) {
            ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i_s].i_type =
                    ps_left_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i_section_nbr - 1 - i_s].i_type;
            ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i_s].i_width =
                    ps_left_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i_section_nbr - 1 - i_s].i_width
                    + (i_s == 0 ? i_right_width - i_left_width : 0);
        }
    }
    return VLC_SUCCESS;
}
