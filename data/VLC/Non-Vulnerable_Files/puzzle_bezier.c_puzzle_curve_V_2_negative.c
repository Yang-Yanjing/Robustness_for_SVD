 *****************************************************************************/
point_t *puzzle_curve_V_2_negative(uint8_t i_pts_nbr, point_t *ps_pt)
{
    if (ps_pt == NULL)
        return NULL;
    point_t *ps_new_pt = malloc( sizeof( point_t ) * (3 * (i_pts_nbr-1) + 1) );
    if (ps_new_pt == NULL)
        return NULL;
    for (uint8_t i=0; i < (3 * (i_pts_nbr-1) + 1); i++) {
        ps_new_pt[i].f_x = -ps_pt[i].f_x;
        ps_new_pt[i].f_y = ps_pt[i].f_y;
    }
    return ps_new_pt;
}
