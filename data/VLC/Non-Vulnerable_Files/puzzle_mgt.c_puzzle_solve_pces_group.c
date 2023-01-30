 *****************************************************************************/
void puzzle_solve_pces_group( filter_t *p_filter) {
    filter_sys_t *p_sys = p_filter->p_sys;
    int32_t i_dx, i_dy;
    p_sys->i_solve_grp_loop++;
    p_sys->i_solve_grp_loop %= p_sys->s_allocated.i_pieces_nbr;
    int32_t i_piece_A = p_sys->i_solve_grp_loop;
    piece_t *ps_piece_A = &p_sys->ps_pieces[i_piece_A];
    for (uint32_t i_piece_B = 0; i_piece_B < p_sys->s_allocated.i_pieces_nbr; i_piece_B++)
    {
        piece_t *ps_piece_B = &p_sys->ps_pieces[i_piece_B];
        if ( ps_piece_A->i_actual_angle != ps_piece_B->i_actual_angle || ps_piece_A->i_actual_mirror != ps_piece_B->i_actual_mirror )
            continue;
        if ( (ps_piece_B->i_group_ID != p_sys->ps_pieces[i_piece_A].i_group_ID ) )
        {
            if ( abs(ps_piece_A->i_OTy - ps_piece_B->i_OTy )<3)
            {
                if (    abs( ps_piece_A->i_ORx - ps_piece_B->i_OLx + 1 ) < 3
                     && abs( ps_piece_A->i_TRx - ps_piece_B->i_TLx + 1 ) < p_sys->i_magnet_accuracy
                     && abs( ps_piece_A->i_TRy - ps_piece_B->i_TLy ) < p_sys->i_magnet_accuracy
                     && abs( ps_piece_A->i_BRx - ps_piece_B->i_BLx + 1 ) < p_sys->i_magnet_accuracy
                     && abs( ps_piece_A->i_BRy - ps_piece_B->i_BLy ) < p_sys->i_magnet_accuracy )
                {
                    i_dx = ps_piece_A->i_TRx - ps_piece_B->i_TLx + ps_piece_A->i_step_x_x;
                    i_dy = ps_piece_A->i_TRy - ps_piece_B->i_TLy;
                    if (!ps_piece_B->b_finished)
                        puzzle_move_group( p_filter, i_piece_B, i_dx, i_dy);
                    else
                        puzzle_move_group( p_filter, i_piece_A, -i_dx, -i_dy);
                    uint32_t i_group_ID = ps_piece_B->i_group_ID;
                    for (uint32_t i_for = 0; i_for < p_sys->s_allocated.i_pieces_nbr; i_for++)
                        if ( p_sys->ps_pieces[i_for].i_group_ID == i_group_ID)
                            p_sys->ps_pieces[i_for].i_group_ID = p_sys->ps_pieces[i_piece_A].i_group_ID;
                }
            }
            else if ( abs(ps_piece_A->i_OLx - ps_piece_B->i_OLx ) < 3 )
            {
                if (    abs(ps_piece_A->i_OBy - ps_piece_B->i_OTy + 1 ) < 3
                     && abs( ps_piece_B->i_TLx - ps_piece_A->i_BLx ) < p_sys->i_magnet_accuracy
                     && abs( ps_piece_B->i_TLy - 1 - ps_piece_A->i_BLy ) < p_sys->i_magnet_accuracy
                     && abs( ps_piece_B->i_TRx - ps_piece_A->i_BRx ) < p_sys->i_magnet_accuracy
                     && abs( ps_piece_B->i_TRy - 1 - ps_piece_A->i_BRy ) < p_sys->i_magnet_accuracy )
                {
                    i_dx = ps_piece_A->i_BLx - ps_piece_B->i_TLx;
                    i_dy = ps_piece_A->i_BLy - ps_piece_B->i_TLy + ps_piece_A->i_step_y_y;
                    if (!ps_piece_B->b_finished)
                        puzzle_move_group( p_filter, i_piece_B, i_dx, i_dy);
                    else
                        puzzle_move_group( p_filter, i_piece_A, -i_dx, -i_dy);
                    uint32_t i_group_ID = ps_piece_B->i_group_ID;
                    for (uint32_t i_for = 0; i_for < p_sys->s_allocated.i_pieces_nbr; i_for++)
                        if ( p_sys->ps_pieces[i_for].i_group_ID == i_group_ID)
                            p_sys->ps_pieces[i_for].i_group_ID = p_sys->ps_pieces[i_piece_A].i_group_ID;
                }
            }
        }
        if ( abs( ps_piece_A->i_OTy - ps_piece_B->i_OTy ) < 3 )
        {
            if (    abs( ps_piece_A->i_ORx - ps_piece_B->i_OLx + 1 ) < 3
                 && abs( ps_piece_A->i_TRx - ps_piece_B->i_TLx + 1 ) < p_sys->i_magnet_accuracy
                 && abs( ps_piece_A->i_TRy - ps_piece_B->i_TLy ) < p_sys->i_magnet_accuracy
                 && abs( ps_piece_A->i_BRx - ps_piece_B->i_BLx + 1 ) < p_sys->i_magnet_accuracy
                 && abs( ps_piece_A->i_BRy - ps_piece_B->i_BLy ) < p_sys->i_magnet_accuracy )
            {
                ps_piece_B->i_left_shape = 0;
                ps_piece_A->i_right_shape = 6;
            }
        }
        else if ( abs( ps_piece_A->i_OLx - ps_piece_B->i_OLx )<3 )
        {
            if (    abs( ps_piece_A->i_OBy - ps_piece_B->i_OTy + 1 )<3
                 && abs( ps_piece_B->i_TLx - ps_piece_A->i_BLx ) < p_sys->i_magnet_accuracy
                 && abs( ps_piece_B->i_TLy - 1 - ps_piece_A->i_BLy ) < p_sys->i_magnet_accuracy
                 && abs( ps_piece_B->i_TRx - ps_piece_A->i_BRx ) < p_sys->i_magnet_accuracy
                 && abs( ps_piece_B->i_TRy - 1 - ps_piece_A->i_BRy ) < p_sys->i_magnet_accuracy )
            {
                ps_piece_B->i_top_shape = 2;
                ps_piece_A->i_btm_shape = 4;
            }
        }
   }
}
