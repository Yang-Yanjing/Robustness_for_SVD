}
void puzzle_free_ps_puzzle_array( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if (p_sys->ps_puzzle_array != NULL) {
        for (int32_t r=0; r < p_sys->s_allocated.i_rows + 1; r++) {
            for (int32_t c=0; c < p_sys->s_allocated.i_cols + 1; c++)
                free( p_sys->ps_puzzle_array[r][c] );
            free( p_sys->ps_puzzle_array[r] );
        }
        free( p_sys->ps_puzzle_array );
    }
    p_sys->ps_puzzle_array = NULL;
    free ( p_sys->ps_desk_planes );
    p_sys->ps_desk_planes = NULL;
    free ( p_sys->ps_pict_planes );
    p_sys->ps_pict_planes = NULL;
    return;
}
