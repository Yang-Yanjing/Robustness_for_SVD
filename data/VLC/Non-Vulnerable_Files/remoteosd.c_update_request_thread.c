}
static void* update_request_thread( void *obj )
{
    filter_t* p_filter = (filter_t*)obj;
    filter_sys_t *p_sys = p_filter->p_sys;
    msg_Dbg( p_filter, "VNC update request thread started" );
    rfbFramebufferUpdateRequestMsg udr;
    udr.type = rfbFramebufferUpdateRequest;
    udr.incremental = 0;
    udr.x = 0;
    udr.y = 0;
    udr.w = htons(p_sys->i_vnc_width);
    udr.h = htons(p_sys->i_vnc_height);
    int w;
    vlc_cleanup_push( update_request_thread_cleanup, p_filter );
    w = write_exact(p_filter, p_sys->i_socket, (char*)&udr,
                    sz_rfbFramebufferUpdateRequestMsg);
    vlc_cleanup_pop();
    if( !w )
    {
        msg_Err( p_filter, "Could not write rfbFramebufferUpdateRequestMsg." );
        update_request_thread_cleanup( p_filter );
        return NULL;
    }
    udr.incremental = 1;
    if( p_sys->b_vnc_poll)
    {
        vlc_cleanup_push( update_request_thread_cleanup, p_filter );
        for( ;; )
        {
            msleep( p_sys->i_vnc_poll_interval * 1000 );
            if( !write_exact(p_filter, p_sys->i_socket, (char*)&udr,
                             sz_rfbFramebufferUpdateRequestMsg))
            {
                msg_Err( p_filter, "Could not write rfbFramebufferUpdateRequestMsg." );
                break;
            }
        }
        vlc_cleanup_run();
    }
    else
    {
        msg_Dbg( p_filter, "VNC polling disabled." );
    }
    msg_Dbg( p_filter, "VNC update request thread ended" );
    return NULL;
}
