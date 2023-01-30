 *****************************************************************************/
int puzzle_bake_pieces_shapes( filter_t *p_filter)
{
/* note:
 *   piece_shape_t **ps_pieces_shapes;  * array [each piece type (PCE_TYPE_NBR  * 4 ( * negative ): top, left,right,btm)][each plane] of piece definition
 *   0 => left border
 *   1 => left border (negative, never used)
 *   2 => top border
 *   .....
 *   8 => bezier left
 *   9 => bezier left negative
 *  10 => bezier top
 *  11 => bezier top negative
 *  12 => bezier btm
 *  13 => bezier btm negative
 *  14 => bezier right
 *  15 => bezier right negative
 *  .....
 */
    filter_sys_t *p_sys = p_filter->p_sys;
    puzzle_free_ps_pieces_shapes(p_filter);
    p_sys->ps_pieces_shapes = malloc( sizeof( piece_shape_t *) * PIECE_TYPE_NBR );
    if( !p_sys->ps_pieces_shapes )
        return VLC_ENOMEM;
    for (int32_t i_piece = 0; i_piece < PIECE_TYPE_NBR; i_piece++) {
        p_sys->ps_pieces_shapes[i_piece] = malloc( sizeof( piece_shape_t) * p_sys->s_allocated.i_planes );
        if( !p_sys->ps_pieces_shapes[i_piece] )
            return VLC_ENOMEM;
        for (uint8_t i_plane = 0; i_plane < p_filter->p_sys->s_allocated.i_planes; i_plane++) {
            p_sys->ps_pieces_shapes[i_piece][i_plane].i_row_nbr = 0;
            p_sys->ps_pieces_shapes[i_piece][i_plane].ps_piece_shape_row = NULL;
        }
    }
    int32_t i_currect_shape = 0;
    for (uint8_t i_plane = 0; i_plane < p_filter->p_sys->s_allocated.i_planes; i_plane++) {
        int i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+0][i_plane], i_plane, puzzle_SHAPE_LEFT);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+1][i_plane], i_plane, puzzle_SHAPE_LEFT);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+2][i_plane], i_plane, puzzle_SHAPE_TOP);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+3][i_plane], i_plane, puzzle_SHAPE_TOP);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+4][i_plane], i_plane, puzzle_SHAPE_BTM);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+5][i_plane], i_plane, puzzle_SHAPE_BTM);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+6][i_plane], i_plane, puzzle_SHAPE_RIGHT);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_ret = puzzle_generate_sect_border( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+7][i_plane], i_plane, puzzle_SHAPE_RIGHT);
        if (i_ret != VLC_SUCCESS) return i_ret;
    }
    i_currect_shape += 8;
    int32_t i_width = p_sys->ps_desk_planes[0].i_pce_max_width;
    int32_t i_lines = p_sys->ps_desk_planes[0].i_pce_max_lines;
    for (int32_t i_shape = 0; i_shape<SHAPES_QTY; i_shape++) {
        point_t *ps_scale_pts_H = puzzle_scale_curve_H(i_width, i_lines,     7, p_sys->ps_bezier_pts_H[i_shape], p_sys->s_allocated.i_shape_size);
        point_t *ps_scale_pts_V = puzzle_H_2_scale_curve_V(i_width, i_lines, 7, p_sys->ps_bezier_pts_H[i_shape], p_sys->s_allocated.i_shape_size);
        point_t *ps_neg_pts_H =   puzzle_curve_H_2_negative(7, ps_scale_pts_H);
        point_t *ps_neg_pts_V =   puzzle_curve_V_2_negative(7, ps_scale_pts_V);
        if (!ps_scale_pts_H || !ps_scale_pts_V || !ps_neg_pts_H || !ps_neg_pts_V) {
            free(ps_scale_pts_H);
            free(ps_scale_pts_V);
            free(ps_neg_pts_H);
            free(ps_neg_pts_V);
            return VLC_EGENERIC;
        }
        int i_ret;
        for (uint8_t i_plane = 0; i_plane < p_filter->p_sys->s_allocated.i_planes; i_plane++) {
            i_ret = puzzle_generate_sect_bezier( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape][i_plane],   7, ps_scale_pts_V, i_plane, puzzle_SHAPE_LEFT);
            if (i_ret != VLC_SUCCESS) break;
            i_ret = puzzle_generate_sect_bezier( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+1][i_plane], 7, ps_neg_pts_V,   i_plane, puzzle_SHAPE_LEFT);
            if (i_ret != VLC_SUCCESS) break;
            i_ret = puzzle_generate_sect_bezier(  p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+2][i_plane], 7, ps_scale_pts_H, i_plane, puzzle_SHAPE_TOP);
            if (i_ret != VLC_SUCCESS) break;
            i_ret = puzzle_generate_sect_bezier(  p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+3][i_plane], 7, ps_neg_pts_H,   i_plane, puzzle_SHAPE_TOP);
            if (i_ret != VLC_SUCCESS) break;
            i_ret = puzzle_generate_sectTop2Btm( p_filter,    &p_sys->ps_pieces_shapes[i_currect_shape+4][i_plane], &p_sys->ps_pieces_shapes[i_currect_shape+2][i_plane], i_plane);
            if (i_ret != VLC_SUCCESS) break;
            i_ret = puzzle_generate_sectTop2Btm( p_filter,    &p_sys->ps_pieces_shapes[i_currect_shape+5][i_plane], &p_sys->ps_pieces_shapes[i_currect_shape+3][i_plane], i_plane);
            if (i_ret != VLC_SUCCESS) break;
            i_ret = puzzle_generate_sectLeft2Right( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+6][i_plane], &p_sys->ps_pieces_shapes[i_currect_shape][i_plane],   i_plane);
            if (i_ret != VLC_SUCCESS) break;
            i_ret = puzzle_generate_sectLeft2Right( p_filter, &p_sys->ps_pieces_shapes[i_currect_shape+7][i_plane], &p_sys->ps_pieces_shapes[i_currect_shape+1][i_plane], i_plane);
            if (i_ret != VLC_SUCCESS) break;
        }
        free(ps_scale_pts_H);
        free(ps_scale_pts_V);
        free(ps_neg_pts_H);
        free(ps_neg_pts_V);
        if (i_ret != VLC_SUCCESS) return i_ret;
        i_currect_shape += 8;
    }
    p_sys->b_shape_init = true;
    return VLC_SUCCESS;
}
