 *****************************************************************************/
int puzzle_generate_shape_lines( filter_t *p_filter, piece_shape_t *ps_piece_shape, int32_t i_min_y, int32_t i_nb_y, float f_x_ratio, float f_y_ratio, point_t *ps_pt, uint8_t i_pts_nbr, uint8_t i_border, uint8_t i_plane)
{
    /* generate data required to draw a line of a piece sector */
    for (int32_t i_y = i_min_y; i_y < i_nb_y + i_min_y; i_y++) {
        int32_t i_row = i_y - i_min_y;
        int32_t pi_sects[MAX_SECT];
        uint8_t i_sect = puzzle_detect_curve( p_filter, i_y, f_x_ratio, f_y_ratio, ps_pt, i_pts_nbr, i_border, i_plane, pi_sects);
        /* ...we have to convert absolute values to offsets and take into account min_curve_x */
        int8_t i_s = 0;
        int32_t i_last_x = (i_border==puzzle_SHAPE_TOP && (i_y>=0))?puzzle_diagonal_limit( p_filter, i_y, true, i_plane ):0;
        for (i_s = 0; i_s<i_sect; i_s++) {
            int32_t i_current_x = pi_sects[i_s];
            int32_t i_delta = i_current_x - i_last_x;
            pi_sects[i_s] = i_delta;
            i_last_x = i_current_x;
        }
        switch (i_border)
        {
          case puzzle_SHAPE_TOP:
            /* ...allocate memory and copy final values */
            /* note for y > 0 we have to ignore the first offset as it is included in "Left" piece shape */
            if ( i_y >= 0 ) {
                ps_piece_shape->ps_piece_shape_row[i_row].i_section_nbr = i_sect;
                ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section = malloc (  sizeof(row_section_t) * i_sect);
                if (!ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section) {
                    for (uint8_t i=0; i<i_row;i++)
                        free(ps_piece_shape->ps_piece_shape_row[i].ps_row_section);
                    free(ps_piece_shape->ps_piece_shape_row);
                    ps_piece_shape->ps_piece_shape_row = NULL;
                    return VLC_ENOMEM;
                }
                for (uint8_t i=0; i < i_sect; i++) {
                    ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i].i_type = i % 2; /* 0 = fill ; 1 = offset */
                    ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i].i_width = pi_sects[i];
                }
            }
            else {
                ps_piece_shape->ps_piece_shape_row[i_row].i_section_nbr = i_sect;
                ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section = malloc (  sizeof(row_section_t) * i_sect);
                if (!ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section) {
                    for (uint8_t i=0; i<i_row;i++)
                        free(ps_piece_shape->ps_piece_shape_row[i].ps_row_section);
                    free(ps_piece_shape->ps_piece_shape_row);
                    ps_piece_shape->ps_piece_shape_row = NULL;
                    return VLC_ENOMEM;
                }
                for (uint8_t i=0; i < i_sect; i++) {
                    ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i].i_type = (i + 1) % 2; /* 0 = fill ; 1 = offset */
                    ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i].i_width = pi_sects[i];
                }
            }
            break;
          case puzzle_SHAPE_LEFT:
            /* ...allocate memory and copy final values */
            /* note for y > 0 we have to ignore the first offset as it is included in "Left" piece shape */
            ps_piece_shape->ps_piece_shape_row[i_row].i_section_nbr = i_sect;
            ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section = malloc (  sizeof(row_section_t) * i_sect);
            if (!ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section) {
                for (uint8_t i=0; i<i_row;i++)
                    free(ps_piece_shape->ps_piece_shape_row[i].ps_row_section);
                free(ps_piece_shape->ps_piece_shape_row);
                ps_piece_shape->ps_piece_shape_row = NULL;
                return VLC_ENOMEM;
            }
            for (uint8_t i=0; i < i_sect; i++) {
                ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i].i_type = (i+1) % 2; /* 0 = fill ; 1 = offset */
                ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section[i].i_width = pi_sects[i];
            }
        }
    }
    return VLC_SUCCESS;
}
