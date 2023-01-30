 *****************************************************************************/
int puzzle_generate_sect_border( filter_t *p_filter, piece_shape_t *ps_piece_shape, uint8_t i_plane, uint8_t i_border)
{
    /* generate data required to draw a sector of border puzzle piece */
    if (!ps_piece_shape)
        return VLC_EGENERIC;
    filter_sys_t *p_sys = p_filter->p_sys;
    int32_t i_width = p_sys->ps_desk_planes[i_plane].i_pce_max_width;
    int32_t i_lines = p_sys->ps_desk_planes[i_plane].i_pce_max_lines;
    /* process each horizontal pixel lines */
    int32_t i_min_y = (i_border != puzzle_SHAPE_BTM) ? 0 : floor( i_lines / 2 );
    int32_t i_nb_y = (i_border != puzzle_SHAPE_TOP)?
                        (i_lines - i_min_y) : (i_lines /2 - i_min_y);
    /* allocate memory */
    ps_piece_shape->i_row_nbr = i_nb_y;
    ps_piece_shape->i_first_row_offset = i_min_y;
    ps_piece_shape->ps_piece_shape_row = malloc( sizeof( piece_shape_row_t ) * i_nb_y );
    if (!ps_piece_shape->ps_piece_shape_row)
        return VLC_ENOMEM;
    for (int32_t i_y = i_min_y; i_y < i_nb_y + i_min_y; i_y++) {
        uint8_t i_sect = 0;
        int32_t pi_sects[MAX_SECT];
        int32_t i_row = i_y - i_min_y;
        /* ...fill from border to next junction */
        switch (i_border)
        {
          case puzzle_SHAPE_TOP:
          case puzzle_SHAPE_BTM:
            pi_sects[i_sect] = puzzle_diagonal_limit( p_filter, i_y, false, i_plane ) - 1
                            - (puzzle_diagonal_limit( p_filter, i_y, true, i_plane ) - 1);
            break;
          case puzzle_SHAPE_RIGHT:
            pi_sects[i_sect] = i_width - puzzle_diagonal_limit( p_filter, i_y, false, i_plane );
            break;
          case puzzle_SHAPE_LEFT:
          default:
            pi_sects[i_sect] = puzzle_diagonal_limit( p_filter, i_y, true, i_plane );
        }
        i_sect++;
        /* ...allocate memory and copy final values */
        ps_piece_shape->ps_piece_shape_row[i_row].i_section_nbr = i_sect;
        ps_piece_shape->ps_piece_shape_row[i_row].ps_row_section = malloc ( sizeof(row_section_t) * i_sect);
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
    return VLC_SUCCESS;
}
