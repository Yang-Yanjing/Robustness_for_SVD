 *****************************************************************************/
int puzzle_Callback( vlc_object_t *p_this, char const *psz_var,
                           vlc_value_t oldval, vlc_value_t newval,
                           void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    vlc_mutex_lock( &p_sys->lock );
    if( !strcmp( psz_var, CFG_PREFIX "rows" ) ) {
        p_sys->s_new_param.i_rows = __MAX( 1, newval.i_int );
    }
    else if( !strcmp( psz_var, CFG_PREFIX "cols" ) ) {
        p_sys->s_new_param.i_cols = __MAX( 1, newval.i_int );
    }
    else if( !strcmp( psz_var, CFG_PREFIX "border" ) ) {
        p_sys->s_new_param.i_border = __MAX( 0, newval.i_int );
    }
    else if( !strcmp( psz_var, CFG_PREFIX "preview" ) ) {
        p_sys->s_new_param.b_preview = newval.b_bool;
    }
    else if( !strcmp( psz_var, CFG_PREFIX "preview-size" ) ) {
        p_sys->s_new_param.i_preview_size = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_PREFIX "shape-size" ) ) {
        p_sys->s_new_param.i_shape_size = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_PREFIX "auto-shuffle" ) ) {
        p_sys->s_new_param.i_auto_shuffle_speed = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_PREFIX "auto-solve" ) ) {
        p_sys->s_new_param.i_auto_solve_speed = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_PREFIX "rotation" ) ) {
        p_sys->s_new_param.i_rotate = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_PREFIX "mode" ) ) {
        p_sys->s_new_param.i_mode = newval.i_int;
    }
    p_sys->b_change_param = true;
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
