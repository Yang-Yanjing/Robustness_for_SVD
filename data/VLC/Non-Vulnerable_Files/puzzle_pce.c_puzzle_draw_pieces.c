 *****************************************************************************/
void puzzle_draw_pieces( filter_t *p_filter, picture_t *p_pic_in, picture_t *p_pic_out)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if ((p_sys->ps_puzzle_array == NULL) || (p_sys->ps_pieces == NULL))
        return;
    for( uint8_t i_plane = 0; i_plane < p_pic_out->i_planes; i_plane++ ) {
        for ( int32_t i = p_sys->s_allocated.i_pieces_nbr-1; i >= 0 ; i-- ) {
            piece_t *ps_piece = &p_sys->ps_pieces[i];
            if (!p_sys->s_current_param.b_advanced
                    || (ps_piece->i_actual_mirror == 1 && ps_piece->i_actual_angle == 0
                    && p_sys->s_current_param.i_shape_size == 0))
            {
                puzzle_drw_basic_pce_in_plane(p_filter, p_pic_in, p_pic_out, i_plane, ps_piece);
            }
            else if ( ( p_sys->s_current_param.i_shape_size == 0)  || !p_sys->b_shape_init
                    || (p_sys->ps_pieces_shapes == NULL) || (!p_sys->b_shape_init) )
            {
                puzzle_drw_adv_pce_in_plane(p_filter, p_pic_in, p_pic_out, i_plane, ps_piece);
            }
            else {
                puzzle_drw_complex_pce_in_plane(p_filter, p_pic_in, p_pic_out, i_plane, ps_piece, i);
            }
        }
    }
    return;
}
