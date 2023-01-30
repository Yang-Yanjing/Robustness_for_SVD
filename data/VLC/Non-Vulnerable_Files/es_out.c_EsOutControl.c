}
static int EsOutControl( es_out_t *out, int i_query, va_list args )
{
    es_out_sys_t *p_sys = out->p_sys;
    int i_ret;
    vlc_mutex_lock( &p_sys->lock );
    i_ret = EsOutControlLocked( out, i_query, args );
    vlc_mutex_unlock( &p_sys->lock );
    return i_ret;
}
