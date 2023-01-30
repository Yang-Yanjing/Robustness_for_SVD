 */
void Close( vlc_object_t *p_this ) {
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_DelCallback( p_filter, CFG_PREFIX "rows",          puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "cols",          puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "border",        puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "preview",       puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "preview-size",  puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "shape-size",    puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "auto-shuffle",  puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "auto-solve",    puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "rotation",      puzzle_Callback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "mode",          puzzle_Callback, p_sys );
    vlc_mutex_destroy( &p_sys->lock );
    vlc_mutex_destroy( &p_sys->pce_lock );
    /* Free allocated memory */
    puzzle_free_ps_puzzle_array ( p_filter );
    puzzle_free_ps_pieces_shapes ( p_filter);
    puzzle_free_ps_pieces ( p_filter );
    free(p_sys->ps_desk_planes);
    free(p_sys->ps_pict_planes);
    free( p_sys->pi_order );
    for (int32_t i_shape = 0; i_shape<SHAPES_QTY; i_shape++)
        free(p_sys->ps_bezier_pts_H[i_shape]);
    free(p_sys->ps_bezier_pts_H);
    free( p_sys );
}
