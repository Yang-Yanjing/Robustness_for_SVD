 *****************************************************************************/
es_out_t *input_EsOutTimeshiftNew( input_thread_t *p_input, es_out_t *p_next_out, int i_rate )
{
    es_out_t *p_out = malloc( sizeof(*p_out) );
    if( !p_out )
        return NULL;
    es_out_sys_t *p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
    {
        free( p_out );
        return NULL;
    }
    /* */
    p_out->pf_add     = Add;
    p_out->pf_send    = Send;
    p_out->pf_del     = Del;
    p_out->pf_control = Control;
    p_out->pf_destroy = Destroy;
    p_out->p_sys      = p_sys;
    /* */
    p_sys->b_input_paused = false;
    p_sys->b_input_paused_source = false;
    p_sys->p_input = p_input;
    p_sys->i_input_rate = i_rate;
    p_sys->i_input_rate_source = i_rate;
    p_sys->p_out = p_next_out;
    vlc_mutex_init_recursive( &p_sys->lock );
    p_sys->b_delayed = false;
    p_sys->p_ts = NULL;
    TAB_INIT( p_sys->i_es, p_sys->pp_es );
    /* */
    const int i_tmp_size_max = var_CreateGetInteger( p_input, "input-timeshift-granularity" );
    if( i_tmp_size_max < 0 )
        p_sys->i_tmp_size_max = 50*1024*1024;
    else
        p_sys->i_tmp_size_max = __MAX( i_tmp_size_max, 1*1024*1024 );
    char *psz_tmp_path = var_CreateGetNonEmptyString( p_input, "input-timeshift-path" );
    p_sys->psz_tmp_path = GetTmpPath( psz_tmp_path );
    msg_Dbg( p_input, "using timeshift granularity of %d MiB, in path '%s'",
             (int)p_sys->i_tmp_size_max/(1024*1024), p_sys->psz_tmp_path );
#if 0
#define S(t) msg_Err( p_input, "SIZEOF("#t")=%d", sizeof(t) )
    S(ts_cmd_t);
    S(ts_cmd_control_t);
    S(ts_cmd_send_t);
    S(ts_cmd_del_t);
    S(ts_cmd_add_t);
#undef S
#endif
    return p_out;
}
