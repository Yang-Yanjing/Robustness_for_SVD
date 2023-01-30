 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    filter_t     *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    /* Stop Goom Thread */
    vlc_mutex_lock( &p_sys->p_thread->lock );
    p_sys->p_thread->b_exit = true;
    vlc_cond_signal( &p_sys->p_thread->wait );
    vlc_mutex_unlock( &p_sys->p_thread->lock );
    vlc_join( p_sys->p_thread->thread, NULL );
    /* Free data */
    aout_filter_RequestVout( p_filter, p_sys->p_thread->p_vout, NULL );
    vlc_mutex_destroy( &p_sys->p_thread->lock );
    vlc_cond_destroy( &p_sys->p_thread->wait );
    while( p_sys->p_thread->i_blocks-- )
    {
        block_Release( p_sys->p_thread->pp_blocks[p_sys->p_thread->i_blocks] );
    }
    free( p_sys->p_thread );
    free( p_sys );
}
