}
static int Control( es_out_t *p_out, int i_query, va_list args )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    int i_ret;
    vlc_mutex_lock( &p_sys->lock );
    TsAutoStop( p_out );
    i_ret = ControlLocked( p_out, i_query, args );
    vlc_mutex_unlock( &p_sys->lock );
    return i_ret;
}
