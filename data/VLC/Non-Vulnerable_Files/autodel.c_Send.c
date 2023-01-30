}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *p_es,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    mtime_t i_current = mdate();
    int i;
    p_es->i_last = p_buffer->i_dts;
    if ( !p_es->id && !p_es->b_error )
    {
        p_es->id = p_stream->p_next->pf_add( p_stream->p_next, &p_es->fmt );
        if ( p_es->id == NULL )
        {
            p_es->b_error = true;
            msg_Err( p_stream, "couldn't create chain for id %d",
                     p_es->fmt.i_id );
        }
    }
    if ( !p_es->b_error )
        p_stream->p_next->pf_send( p_stream->p_next, p_es->id, p_buffer );
    else
        block_ChainRelease( p_buffer );
    for ( i = 0; i < p_sys->i_es_num; i++ )
    {
        if ( p_sys->pp_es[i]->id != NULL
              && (p_sys->pp_es[i]->fmt.i_cat == VIDEO_ES
                   || p_sys->pp_es[i]->fmt.i_cat == AUDIO_ES)
              && p_sys->pp_es[i]->i_last < i_current )
        {
            p_stream->p_next->pf_del( p_stream->p_next, p_sys->pp_es[i]->id );
            p_sys->pp_es[i]->id = NULL;
        }
    }
    return VLC_SUCCESS;
}
