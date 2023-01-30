}
void puzzle_random_rotate( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    /* add random rotation to each piece */
    for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++)
    {
        p_sys->ps_pieces[i].i_actual_angle  = 0;
        p_sys->ps_pieces[i].i_actual_mirror = +1;
        switch ( p_sys->s_current_param.i_rotate )
        {
          case 1:
                puzzle_rotate_pce( p_filter, i, (( (unsigned) vlc_mrand48()) % ( 2 ) ) * 2, p_sys->ps_pieces[i].i_center_x, p_sys->ps_pieces[i].i_center_y, false );
            break;
          case 2:
                puzzle_rotate_pce( p_filter, i, (( (unsigned) vlc_mrand48()) % ( 4 ) ), p_sys->ps_pieces[i].i_center_x, p_sys->ps_pieces[i].i_center_y, false );
            break;
          case 3:
                puzzle_rotate_pce( p_filter, i, (( (unsigned) vlc_mrand48()) % ( 8 ) ), p_sys->ps_pieces[i].i_center_x, p_sys->ps_pieces[i].i_center_y, false );
            break;
        }
        puzzle_calculate_corners( p_filter, i );
    }
}
