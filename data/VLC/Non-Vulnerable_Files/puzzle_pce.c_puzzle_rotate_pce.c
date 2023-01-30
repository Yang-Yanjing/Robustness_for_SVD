 *****************************************************************************/
void puzzle_rotate_pce( filter_t *p_filter, int32_t i_piece, int8_t i_rotate_mirror, int32_t i_center_x, int32_t i_center_y, bool b_avoid_mirror )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    piece_t *ps_piece = &p_sys->ps_pieces[i_piece];
    if ( p_sys->s_current_param.i_rotate == 0 )
        return;
    if ( p_sys->s_current_param.i_rotate == 1 && (i_rotate_mirror != 2) )
        return;
    for ( uint8_t i=0; i < abs( i_rotate_mirror ); i++) {
        int32_t i_tempx, i_tempy;
        /* piece has to be rotated by 90° */
        if ( i_rotate_mirror > 0 ) {
            ps_piece->i_actual_angle++;
            ps_piece->i_actual_angle &= 0x03;
            i_tempx = -( i_center_y - ps_piece->ps_piece_in_plane[0].i_actual_y ) + i_center_x;
            i_tempy = +( i_center_x - ps_piece->ps_piece_in_plane[0].i_actual_x ) + i_center_y;
        }
        else {
            ps_piece->i_actual_angle--;
            ps_piece->i_actual_angle &= 0x03;
            i_tempx = +( i_center_y - ps_piece->ps_piece_in_plane[0].i_actual_y ) + i_center_x;
            i_tempy = -( i_center_x - ps_piece->ps_piece_in_plane[0].i_actual_x ) + i_center_y;
        }
        ps_piece->ps_piece_in_plane[0].i_actual_x = i_tempx;
        ps_piece->ps_piece_in_plane[0].i_actual_y = i_tempy;
        if ( ps_piece->i_actual_angle == 0 && p_sys->s_current_param.i_rotate == 3 && !b_avoid_mirror ) {
            ps_piece->ps_piece_in_plane[0].i_actual_x = 2 * i_center_x - ps_piece->ps_piece_in_plane[0].i_actual_x;
            ps_piece->i_actual_mirror *= -1;
        }
        puzzle_calculate_corners( p_filter, i_piece );
    }
}
