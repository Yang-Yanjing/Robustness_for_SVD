}
static void DecoderDecodeAudio( decoder_t *p_dec, block_t *p_block )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    block_t *p_aout_buf;
    int i_decoded = 0;
    int i_lost = 0;
    int i_played = 0;
    if (!p_block) {
        /* Play a NULL block to output buffered frames */
        DecoderPlayAudio( p_dec, NULL, &i_played, &i_lost );
    }
    else while( (p_aout_buf = p_dec->pf_decode_audio( p_dec, &p_block )) )
    {
        if( DecoderIsExitRequested( p_dec ) )
        {
            /* It prevent freezing VLC in case of broken decoder */
            block_Release( p_aout_buf );
            if( p_block )
                block_Release( p_block );
            break;
        }
        i_decoded++;
        if( p_owner->i_preroll_end > VLC_TS_INVALID &&
            p_aout_buf->i_pts < p_owner->i_preroll_end )
        {
            block_Release( p_aout_buf );
            continue;
        }
        if( p_owner->i_preroll_end > VLC_TS_INVALID )
        {
            msg_Dbg( p_dec, "End of audio preroll" );
            if( p_owner->p_aout )
                aout_DecFlush( p_owner->p_aout );
            /* */
            p_owner->i_preroll_end = VLC_TS_INVALID;
        }
        DecoderPlayAudio( p_dec, p_aout_buf, &i_played, &i_lost );
    }
    /* Update ugly stat */
    input_thread_t  *p_input = p_owner->p_input;
    if( p_input != NULL && (i_decoded > 0 || i_lost > 0 || i_played > 0) )
    {
        vlc_mutex_lock( &p_input->p->counters.counters_lock);
        stats_Update( p_input->p->counters.p_lost_abuffers, i_lost, NULL );
        stats_Update( p_input->p->counters.p_played_abuffers, i_played, NULL );
        stats_Update( p_input->p->counters.p_decoded_audio, i_decoded, NULL );
        vlc_mutex_unlock( &p_input->p->counters.counters_lock);
    }
}
