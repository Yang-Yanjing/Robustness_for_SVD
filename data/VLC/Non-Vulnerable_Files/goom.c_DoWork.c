 *****************************************************************************/
static block_t *DoWork( filter_t *p_filter, block_t *p_in_buf )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    block_t *p_block;
    /* Queue sample */
    vlc_mutex_lock( &p_sys->p_thread->lock );
    if( p_sys->p_thread->i_blocks == MAX_BLOCKS )
    {
        vlc_mutex_unlock( &p_sys->p_thread->lock );
        return p_in_buf;
    }
    p_block = block_Alloc( p_in_buf->i_buffer );
    if( !p_block )
    {
        vlc_mutex_unlock( &p_sys->p_thread->lock );
        return p_in_buf;
    }
    memcpy( p_block->p_buffer, p_in_buf->p_buffer, p_in_buf->i_buffer );
    p_block->i_pts = p_in_buf->i_pts;
    p_sys->p_thread->pp_blocks[p_sys->p_thread->i_blocks++] = p_block;
    vlc_cond_signal( &p_sys->p_thread->wait );
    vlc_mutex_unlock( &p_sys->p_thread->lock );
    return p_in_buf;
}
