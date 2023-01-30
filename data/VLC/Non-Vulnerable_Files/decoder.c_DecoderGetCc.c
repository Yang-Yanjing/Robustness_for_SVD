}
static void DecoderGetCc( decoder_t *p_dec, decoder_t *p_dec_cc )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    block_t *p_cc;
    bool pb_present[4];
    bool b_processed = false;
    int i;
    int i_cc_decoder;
    assert( p_dec_cc->pf_get_cc != NULL );
    /* Do not try retreiving CC if not wanted (sout) or cannot be retreived */
    if( !p_owner->cc.b_supported )
        return;
    p_cc = p_dec_cc->pf_get_cc( p_dec_cc, pb_present );
    if( !p_cc )
        return;
    vlc_mutex_lock( &p_owner->lock );
    for( i = 0, i_cc_decoder = 0; i < 4; i++ )
    {
        p_owner->cc.pb_present[i] |= pb_present[i];
        if( p_owner->cc.pp_decoder[i] )
            i_cc_decoder++;
    }
    for( i = 0; i < 4; i++ )
    {
        if( !p_owner->cc.pp_decoder[i] )
            continue;
        block_FifoPut( p_owner->cc.pp_decoder[i]->p_owner->p_fifo,
            (i_cc_decoder > 1) ? block_Duplicate(p_cc) : p_cc);
        i_cc_decoder--;
        b_processed = true;
    }
    vlc_mutex_unlock( &p_owner->lock );
    if( !b_processed )
        block_Release( p_cc );
}
