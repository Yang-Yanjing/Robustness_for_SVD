}
static void TsAutoStop( es_out_t *p_out )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    if( !p_sys->b_delayed || !TsIsUnused( p_sys->p_ts ) )
        return;
    msg_Warn( p_sys->p_input, "es out timeshift: auto stop" );
    TsStop( p_sys->p_ts );
    p_sys->b_delayed = false;
}
