}
static bool DecoderWaitUnblock( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_assert_locked( &p_owner->lock );
    for( ;; )
    {
        if( p_owner->b_flushing )
            break;
        if( p_owner->b_paused )
        {
            if( p_owner->b_waiting && !p_owner->b_has_data )
                break;
            if( p_owner->pause.i_ignore > 0 )
            {
                p_owner->pause.i_ignore--;
                break;
            }
        }
        else
        {
            if( !p_owner->b_waiting || !p_owner->b_has_data )
                break;
        }
        vlc_cond_wait( &p_owner->wait_request, &p_owner->lock );
    }
    return p_owner->b_flushing;
}
