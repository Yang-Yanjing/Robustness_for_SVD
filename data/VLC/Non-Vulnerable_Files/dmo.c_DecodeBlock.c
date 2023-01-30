}
static void *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    void *p_ret;
    vlc_mutex_lock( &p_sys->lock );
    if( p_sys->i_output <= 0 )
    {
        p_sys->pp_input = pp_block;
        vlc_cond_signal( &p_sys->wait_input );
        while( p_sys->pp_input )
            vlc_cond_wait( &p_sys->wait_output, &p_sys->lock );
    }
    p_ret = NULL;
    if( p_sys->i_output > 0 )
    {
        p_ret = p_sys->pp_output[0];
        TAB_REMOVE( p_sys->i_output, p_sys->pp_output, p_ret );
    }
    vlc_mutex_unlock( &p_sys->lock );
    return p_ret;
}
