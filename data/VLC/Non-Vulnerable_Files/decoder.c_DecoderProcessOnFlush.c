/* */
static void DecoderProcessOnFlush( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    if( p_owner->b_flushing )
    {
        p_owner->b_flushing = false;
        vlc_cond_signal( &p_owner->wait_acknowledge );
    }
    vlc_mutex_unlock( &p_owner->lock );
}
