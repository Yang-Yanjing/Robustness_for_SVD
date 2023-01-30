}
static void LoadMpls( demux_t *p_demux, const char *psz_name, int i_id )
{
    demux_sys_t *p_sys = p_demux->p_sys;
#if defined(BD_DEBUG)
    msg_Err( p_demux, "Loading %s", psz_name );
#endif
    block_t *p_block = LoadBlock( p_demux, psz_name );
    if( !p_block )
        goto error;
    /* */
    bd_mpls_t *p_mpls = malloc( sizeof(*p_mpls) );
    if( !p_mpls )
        goto error;
    /* */
    bs_t s;
    bs_init( &s, p_block->p_buffer, p_block->i_buffer );
    if( bd_mpls_Parse( p_mpls, &s, i_id ) )
        goto error;
#if defined(BD_DEBUG)
    msg_Err( p_demux, "MPLS: id=%d", p_mpls->i_id );
    msg_Err( p_demux, "MPLS: play_item=%d sub_path=%d",
             p_mpls->i_play_item, p_mpls->i_sub_path );
    for( int i = 0; i < p_mpls->i_play_item; i++ )
    {
        bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[i];
        msg_Err( p_demux, "PLAY_ITEM[%d] connection=%d in=%d out=%d still=%d(%d)",
                 i, p_item->i_connection,
                 (int)p_item->i_in_time, (int)p_item->i_out_time,
                 p_item->i_still, p_item->i_still_time );
        msg_Err( p_demux, "     clpi_default: id=%d stc_id=%d",
                 p_item->clpi.i_id, p_item->clpi.i_stc_id );
        for( int j = 0; j < p_item->i_clpi; j++ )
            msg_Err( p_demux, "     clpi[%d]: id=%d stc_id=%d",
                     j, p_item->p_clpi[j].i_id, p_item->p_clpi[j].i_stc_id );
        for( int j = 0; j < p_item->i_stream; j++ )
            msg_Err( p_demux, "     stream[%d]: type=%d class=%d stream_type=0x%x lang=%s charset=%d",
                     j,
                     p_item->p_stream[j].i_type,
                     p_item->p_stream[j].i_class,
                     p_item->p_stream[j].i_stream_type,
                     p_item->p_stream[j].psz_language,
                     p_item->p_stream[j].i_charset );
    }
    for( int i = 0; i < p_mpls->i_sub_path; i++ )
    {
        bd_mpls_sub_path_t *p_sub = &p_mpls->p_sub_path[i];
        msg_Err( p_demux, "SUB_PATH[%d] type=%d repeat=%d item=%d",
                 i, p_sub->i_type, p_sub->b_repeat, p_sub->i_item );
    }
    for( int i = 0; i < p_mpls->i_mark; i++ )
    {
        bd_mpls_mark_t *p_mark = &p_mpls->p_mark[i];
        msg_Err( p_demux, "M[%d] t=%d play_item_id=%d time=%d entry_es_pid=%d",
                 i, p_mark->i_type, p_mark->i_play_item_id, (int)p_mark->i_time, p_mark->i_entry_es_pid );
    }
#endif
    /* */
    TAB_APPEND( p_sys->i_mpls, p_sys->pp_mpls, p_mpls );
    /* */
    block_Release( p_block );
    return;
error:
    msg_Err( p_demux, "Failed loading %s", psz_name );
    if( p_block )
        block_Release( p_block );
}
