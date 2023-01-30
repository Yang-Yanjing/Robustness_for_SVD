/* free allocated shapes data */
void puzzle_free_ps_pieces_shapes( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if (p_sys->ps_pieces_shapes == NULL)
        return;
    for (int32_t p = 0; p < p_sys->s_allocated.i_piece_types; p++) {
        for (uint8_t i_plane = 0; i_plane < p_sys->s_allocated.i_planes; i_plane++) {
            for (int32_t r = 0; r < p_sys->ps_pieces_shapes[p][i_plane].i_row_nbr; r++)
                free( p_sys->ps_pieces_shapes[p][i_plane].ps_piece_shape_row[r].ps_row_section );
            free( p_sys->ps_pieces_shapes[p][i_plane].ps_piece_shape_row );
        }
        free( p_sys->ps_pieces_shapes[p] );
    }
    free( p_sys->ps_pieces_shapes );
    p_sys->ps_pieces_shapes = NULL;
}
