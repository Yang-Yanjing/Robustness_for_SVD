}
static void DecoderDecodeVideo( decoder_t *p_dec, block_t *p_block )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    picture_t      *p_pic;
    int i_lost = 0;
    int i_decoded = 0;
    int i_displayed = 0;
    while( (p_pic = p_dec->pf_decode_video( p_dec, &p_block )) )
    {
        vout_thread_t  *p_vout = p_owner->p_vout;
        if( DecoderIsExitRequested( p_dec ) )
        {
            /* It prevent freezing VLC in case of broken decoder */
            vout_ReleasePicture( p_vout, p_pic );
            if( p_block )
                block_Release( p_block );
            break;
        }
        i_decoded++;
        if( p_owner->i_preroll_end > VLC_TS_INVALID && p_pic->date < p_owner->i_preroll_end )
        {
            vout_ReleasePicture( p_vout, p_pic );
            continue;
        }
        if( p_owner->i_preroll_end > VLC_TS_INVALID )
        {
            msg_Dbg( p_dec, "End of video preroll" );
            if( p_vout )
                vout_Flush( p_vout, VLC_TS_INVALID+1 );
            /* */
            p_owner->i_preroll_end = VLC_TS_INVALID;
        }
        if( p_dec->pf_get_cc &&
            ( !p_owner->p_packetizer || !p_owner->p_packetizer->pf_get_cc ) )
            DecoderGetCc( p_dec, p_dec );
        DecoderPlayVideo( p_dec, p_pic, &i_displayed, &i_lost );
    }
    /* Update ugly stat */
    input_thread_t *p_input = p_owner->p_input;
    if( p_input != NULL && (i_decoded > 0 || i_lost > 0 || i_displayed > 0) )
    {
        vlc_mutex_lock( &p_input->p->counters.counters_lock );
        stats_Update( p_input->p->counters.p_decoded_video, i_decoded, NULL );
        stats_Update( p_input->p->counters.p_lost_pictures, i_lost , NULL);
        stats_Update( p_input->p->counters.p_displayed_pictures,
                      i_displayed, NULL);
        vlc_mutex_unlock( &p_input->p->counters.counters_lock );
    }
}
