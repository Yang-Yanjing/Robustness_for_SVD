}
static int SendIn( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                   block_t *p_buffer )
{
    in_sout_stream_sys_t *p_sys = (in_sout_stream_sys_t *)p_stream->p_sys;
    bridge_t *p_bridge;
    bool b_no_es = true;
    int i;
    int i_date = mdate();
    /* First forward the packet for our own ES */
    if( !p_sys->b_placeholder )
        p_stream->p_next->pf_send( p_stream->p_next, id->id, p_buffer );
    /* Then check all bridged streams */
    vlc_mutex_lock( &lock );
    p_bridge = var_GetAddress( p_stream->p_libvlc, p_sys->psz_name );
    if( p_bridge )
    {
    for ( i = 0; i < p_bridge->i_es_num; i++ )
    {
        if ( !p_bridge->pp_es[i]->b_empty )
            b_no_es = false;
        while ( p_bridge->pp_es[i]->p_block != NULL
                 && (p_bridge->pp_es[i]->p_block->i_dts + p_sys->i_delay
                       < i_date
                      || p_bridge->pp_es[i]->p_block->i_dts + p_sys->i_delay
                          < p_bridge->pp_es[i]->i_last) )
        {
            block_t *p_block = p_bridge->pp_es[i]->p_block;
            msg_Dbg( p_stream, "dropping a packet (%"PRId64 ")",
                     i_date - p_block->i_dts - p_sys->i_delay );
            p_bridge->pp_es[i]->p_block
                = p_bridge->pp_es[i]->p_block->p_next;
            block_Release( p_block );
        }
        if ( p_bridge->pp_es[i]->p_block == NULL )
        {
            p_bridge->pp_es[i]->pp_last = &p_bridge->pp_es[i]->p_block;
        }
        if ( p_bridge->pp_es[i]->b_changed )
        {
            if ( p_bridge->pp_es[i]->b_empty && p_bridge->pp_es[i]->id != NULL )
            {
                p_stream->p_next->pf_del( p_stream->p_next, p_bridge->pp_es[i]->id );
            }
            else
            {
                /* We need at least two packets to enter the mux. */
                if ( p_bridge->pp_es[i]->p_block == NULL
                      || p_bridge->pp_es[i]->p_block->p_next == NULL )
                {
                    continue;
                }
                p_bridge->pp_es[i]->fmt.i_id += p_sys->i_id_offset;
                if( !p_sys->b_placeholder )
                {
                    p_bridge->pp_es[i]->id = p_stream->p_next->pf_add(
                                p_stream->p_next, &p_bridge->pp_es[i]->fmt );
                    if ( p_bridge->pp_es[i]->id == NULL )
                    {
                        msg_Warn( p_stream, "couldn't create chain for id %d",
                                  p_bridge->pp_es[i]->fmt.i_id );
                    }
                }
                msg_Dbg( p_stream, "bridging in input codec=%4.4s id=%d pos=%d",
                         (char*)&p_bridge->pp_es[i]->fmt.i_codec,
                         p_bridge->pp_es[i]->fmt.i_id, i );
            }
        }
        p_bridge->pp_es[i]->b_changed = false;
        if ( p_bridge->pp_es[i]->b_empty )
            continue;
        if ( p_bridge->pp_es[i]->p_block == NULL )
        {
            if ( p_bridge->pp_es[i]->id != NULL
                  && p_bridge->pp_es[i]->i_last < i_date )
            {
                if( !p_sys->b_placeholder )
                    p_stream->p_next->pf_del( p_stream->p_next,
                                          p_bridge->pp_es[i]->id );
                p_bridge->pp_es[i]->fmt.i_id -= p_sys->i_id_offset;
                p_bridge->pp_es[i]->b_changed = true;
                p_bridge->pp_es[i]->id = NULL;
            }
            continue;
        }
        if ( p_bridge->pp_es[i]->id != NULL || p_sys->b_placeholder)
        {
            block_t *p_block = p_bridge->pp_es[i]->p_block;
            while ( p_block != NULL )
            {
                p_bridge->pp_es[i]->i_last = p_block->i_dts;
                p_block->i_pts += p_sys->i_delay;
                p_block->i_dts += p_sys->i_delay;
                p_block = p_block->p_next;
            }
            sout_stream_id_sys_t *newid = NULL;
            if( p_sys->b_placeholder )
            {
                switch( p_bridge->pp_es[i]->fmt.i_cat )
                {
                    case VIDEO_ES:
                        p_sys->i_last_video = i_date;
                        newid = p_sys->id_video;
                        if( !newid )
                            break;
                        if( !p_sys->b_switch_on_iframe ||
                            p_sys->i_state == placeholder_off ||
                            ( p_bridge->pp_es[i]->fmt.i_cat == VIDEO_ES &&
                              p_bridge->pp_es[i]->p_block->i_flags & BLOCK_FLAG_TYPE_I ) )
                        {
                            p_stream->p_next->pf_send( p_stream->p_next,
                                       newid,
                                       p_bridge->pp_es[i]->p_block );
                            p_sys->i_state = placeholder_off;
                        }
                        break;
                    case AUDIO_ES:
                        newid = p_sys->id_audio;
                        if( !newid )
                            break;
                        p_sys->i_last_audio = i_date;
                    default:
                        p_stream->p_next->pf_send( p_stream->p_next,
                                   newid?newid:p_bridge->pp_es[i]->id,
                                   p_bridge->pp_es[i]->p_block );
                        break;
                }
            }
            else /* !b_placeholder */
                p_stream->p_next->pf_send( p_stream->p_next,
                                       p_bridge->pp_es[i]->id,
                                       p_bridge->pp_es[i]->p_block );
        }
        else
        {
            block_ChainRelease( p_bridge->pp_es[i]->p_block );
        }
        p_bridge->pp_es[i]->p_block = NULL;
        p_bridge->pp_es[i]->pp_last = &p_bridge->pp_es[i]->p_block;
    }
    if( b_no_es )
    {
        for ( i = 0; i < p_bridge->i_es_num; i++ )
            free( p_bridge->pp_es[i] );
        free( p_bridge->pp_es );
        free( p_bridge );
        var_Destroy( p_stream->p_libvlc, p_sys->psz_name );
    }
    }
    if( p_sys->b_placeholder )
    {
        switch( id->i_cat )
        {
            case VIDEO_ES:
                if( ( p_sys->i_last_video + p_sys->i_placeholder_delay < i_date
                    && (  !p_sys->b_switch_on_iframe
                       || p_buffer->i_flags & BLOCK_FLAG_TYPE_I ) )
                  || p_sys->i_state == placeholder_on )
                {
                    p_stream->p_next->pf_send( p_stream->p_next, id->id, p_buffer );
                    p_sys->i_state = placeholder_on;
                }
                else
                    block_Release( p_buffer );
                break;
            case AUDIO_ES:
                if( p_sys->i_last_audio + p_sys->i_placeholder_delay < i_date )
                    p_stream->p_next->pf_send( p_stream->p_next, id->id, p_buffer );
                else
                    block_Release( p_buffer );
                break;
            default:
                block_Release( p_buffer ); /* FIXME: placeholder subs anyone? */
                break;
        }
    }
    vlc_mutex_unlock( &lock );
    return VLC_SUCCESS;
}
