 */
static int EsOutSend( es_out_t *out, es_out_id_t *es, block_t *p_block )
{
    es_out_sys_t   *p_sys = out->p_sys;
    input_thread_t *p_input = p_sys->p_input;
    if( libvlc_stats( p_input ) )
    {
        uint64_t i_total;
        vlc_mutex_lock( &p_input->p->counters.counters_lock );
        stats_Update( p_input->p->counters.p_demux_read,
                      p_block->i_buffer, &i_total );
        stats_Update( p_input->p->counters.p_demux_bitrate, i_total, NULL );
        /* Update number of corrupted data packats */
        if( p_block->i_flags & BLOCK_FLAG_CORRUPTED )
        {
            stats_Update( p_input->p->counters.p_demux_corrupted, 1, NULL );
        }
        /* Update number of discontinuities */
        if( p_block->i_flags & BLOCK_FLAG_DISCONTINUITY )
        {
            stats_Update( p_input->p->counters.p_demux_discontinuity, 1, NULL );
        }
        vlc_mutex_unlock( &p_input->p->counters.counters_lock );
    }
    vlc_mutex_lock( &p_sys->lock );
    /* Mark preroll blocks */
    if( p_sys->i_preroll_end >= 0 )
    {
        int64_t i_date = p_block->i_pts;
        if( p_block->i_pts <= VLC_TS_INVALID )
            i_date = p_block->i_dts;
        if( i_date < p_sys->i_preroll_end )
            p_block->i_flags |= BLOCK_FLAG_PREROLL;
    }
    if( !es->p_dec )
    {
        block_Release( p_block );
        vlc_mutex_unlock( &p_sys->lock );
        return VLC_SUCCESS;
    }
    /* Check for sout mode */
    if( p_input->p->p_sout )
    {
        /* FIXME review this, proper lock may be missing */
        if( p_input->p->p_sout->i_out_pace_nocontrol > 0 &&
            p_input->p->b_out_pace_control )
        {
            msg_Dbg( p_input, "switching to sync mode" );
            p_input->p->b_out_pace_control = false;
        }
        else if( p_input->p->p_sout->i_out_pace_nocontrol <= 0 &&
                 !p_input->p->b_out_pace_control )
        {
            msg_Dbg( p_input, "switching to async mode" );
            p_input->p->b_out_pace_control = true;
        }
    }
    /* Decode */
    if( es->p_dec_record )
    {
        block_t *p_dup = block_Duplicate( p_block );
        if( p_dup )
            input_DecoderDecode( es->p_dec_record, p_dup,
                                 p_input->p->b_out_pace_control );
    }
    input_DecoderDecode( es->p_dec, p_block,
                         p_input->p->b_out_pace_control );
    es_format_t fmt_dsc;
    vlc_meta_t  *p_meta_dsc;
    if( input_DecoderHasFormatChanged( es->p_dec, &fmt_dsc, &p_meta_dsc ) )
    {
        EsOutUpdateInfo( out, es, &fmt_dsc, p_meta_dsc );
        es_format_Clean( &fmt_dsc );
        if( p_meta_dsc )
            vlc_meta_Delete( p_meta_dsc );
    }
    /* Check CC status */
    bool pb_cc[4];
    input_DecoderIsCcPresent( es->p_dec, pb_cc );
    for( int i = 0; i < 4; i++ )
    {
        es_format_t fmt;
        if(  es->pb_cc_present[i] || !pb_cc[i] )
            continue;
        msg_Dbg( p_input, "Adding CC track %d for es[%d]", 1+i, es->i_id );
        es_format_Init( &fmt, SPU_ES, EsOutFourccClosedCaptions[i] );
        fmt.i_group = es->fmt.i_group;
        if( asprintf( &fmt.psz_description,
                      _("Closed captions %u"), 1 + i ) == -1 )
            fmt.psz_description = NULL;
        es->pp_cc_es[i] = EsOutAdd( out, &fmt );
        es->pp_cc_es[i]->p_master = es;
        es_format_Clean( &fmt );
        /* */
        es->pb_cc_present[i] = true;
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
