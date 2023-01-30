}
static sout_stream_id_sys_t * AddOut( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    out_sout_stream_sys_t *p_sys = (out_sout_stream_sys_t *)p_stream->p_sys;
    bridge_t *p_bridge;
    bridged_es_t *p_es;
    int i;
    if ( p_sys->b_inited )
    {
        msg_Err( p_stream, "bridge-out can only handle 1 es at a time." );
        return NULL;
    }
    p_sys->b_inited = true;
    vlc_mutex_lock( &lock );
    p_bridge = var_GetAddress( p_stream->p_libvlc, p_sys->psz_name );
    if ( p_bridge == NULL )
    {
        p_bridge = xmalloc( sizeof( bridge_t ) );
        var_Create( p_stream->p_libvlc, p_sys->psz_name, VLC_VAR_ADDRESS );
        var_SetAddress( p_stream->p_libvlc, p_sys->psz_name, p_bridge );
        p_bridge->i_es_num = 0;
        p_bridge->pp_es = NULL;
    }
    for ( i = 0; i < p_bridge->i_es_num; i++ )
    {
        if ( p_bridge->pp_es[i]->b_empty && !p_bridge->pp_es[i]->b_changed )
            break;
    }
    if ( i == p_bridge->i_es_num )
    {
        p_bridge->pp_es = xrealloc( p_bridge->pp_es,
                          (p_bridge->i_es_num + 1) * sizeof(bridged_es_t *) );
        p_bridge->i_es_num++;
        p_bridge->pp_es[i] = xmalloc( sizeof(bridged_es_t) );
    }
    p_sys->p_es = p_es = p_bridge->pp_es[i];
    p_es->fmt = *p_fmt;
    p_es->fmt.i_id = p_sys->i_id;
    p_es->p_block = NULL;
    p_es->pp_last = &p_es->p_block;
    p_es->b_empty = false;
    p_es->id = NULL;
    p_es->i_last = VLC_TS_INVALID;
    p_es->b_changed = true;
    msg_Dbg( p_stream, "bridging out input codec=%4.4s id=%d pos=%d",
             (char*)&p_es->fmt.i_codec, p_es->fmt.i_id, i );
    vlc_mutex_unlock( &lock );
    return (sout_stream_id_sys_t *)p_sys;
}
