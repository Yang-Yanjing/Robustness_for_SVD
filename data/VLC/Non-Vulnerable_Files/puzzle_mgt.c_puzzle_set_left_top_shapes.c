 *****************************************************************************/
void puzzle_set_left_top_shapes( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    /* for each piece on the desk... */
    for (uint16_t i_pce_B=0; i_pce_B < p_sys->s_allocated.i_pieces_nbr; i_pce_B++)
        /* ...search adjacent piece */
        for (uint16_t i_pce_A=0;     i_pce_A < p_sys->s_allocated.i_pieces_nbr; i_pce_A++)
        {
            if (     ( p_sys->ps_pieces[i_pce_A].i_original_row == p_sys->ps_pieces[i_pce_B].i_original_row )
                  && ( p_sys->ps_pieces[i_pce_A].i_original_col == p_sys->ps_pieces[i_pce_B].i_original_col-1 ) )
                /* left shape is based on negative (invert ID LSB) right one */
                p_sys->ps_pieces[i_pce_B].i_left_shape = (p_sys->ps_pieces[i_pce_A].i_right_shape - 6 ) ^ 0x01;
            if (     ( p_sys->ps_pieces[i_pce_A].i_original_row == p_sys->ps_pieces[i_pce_B].i_original_row - 1 )
                  && ( p_sys->ps_pieces[i_pce_A].i_original_col == p_sys->ps_pieces[i_pce_B].i_original_col ) )
                /* top shape is based on negative of bottom one
                 *                 top ID = bottom ID - 2
                 *                 negative ID = invert LSB
                 */
                p_sys->ps_pieces[i_pce_B].i_top_shape = (p_sys->ps_pieces[i_pce_A].i_btm_shape - 2 ) ^ 0x01;
        }
}
