 *****************************************************************************/
int32_t puzzle_diagonal_limit( filter_t *p_filter, int32_t i_y, bool b_left, uint8_t i_plane )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if (b_left ^ (i_y >= p_sys->ps_desk_planes[i_plane].i_pce_max_lines / 2))
        return ( i_y * p_sys->ps_desk_planes[i_plane].i_pce_max_width) / p_sys->ps_desk_planes[i_plane].i_pce_max_lines;
    else
        return p_sys->ps_desk_planes[i_plane].i_pce_max_width - ( ( i_y * p_sys->ps_desk_planes[i_plane].i_pce_max_width) / p_sys->ps_desk_planes[i_plane].i_pce_max_lines);
}
