}
static block_t *Block( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    block_t *p_block = NULL;
    vlc_mutex_lock( &p_sys->lock );
    p_block = p_sys->p_frame;
    //msg_Dbg( p_access, "sending frame %p",p_block );
    p_sys->p_frame = NULL;
    vlc_mutex_unlock( &p_sys->lock );
    return p_block;
}
