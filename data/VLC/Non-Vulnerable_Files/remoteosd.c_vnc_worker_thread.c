}
static void* vnc_worker_thread( void *obj )
{
    filter_t* p_filter = (filter_t*)obj;
    filter_sys_t *p_sys = p_filter->p_sys;
    vlc_thread_t update_request_thread_handle;
    int canc = vlc_savecancel ();
    msg_Dbg( p_filter, "VNC worker thread started" );
    if( !open_vnc_connection ( p_filter ) )
    {
        msg_Err( p_filter, "Could not connect to vnc host" );
        goto exit;
    }
    if( !handshaking ( p_filter ) )
    {
        msg_Err( p_filter, "Error occurred while handshaking vnc host" );
        goto exit;
    }
    p_sys->b_connection_active = true; /* to enable sending key
                                            * and mouse events to host */
    /* Create an empty picture for VNC the data */
    vlc_mutex_lock( &p_sys->lock );
    p_sys->p_pic = picture_New( VLC_CODEC_YUVA,
                                p_sys->i_vnc_width, p_sys->i_vnc_height, 1, 1 );
    if( !p_sys->p_pic )
    {
        vlc_mutex_unlock( &p_sys->lock );
        goto exit;
    }
    p_sys->i_vnc_pixels = p_sys->i_vnc_width * p_sys->i_vnc_height;
    vlc_mutex_unlock( &p_sys->lock );
    /* create the update request thread */
    if( vlc_clone( &update_request_thread_handle,
                   update_request_thread, p_filter,
                   VLC_THREAD_PRIORITY_LOW ) )
    {
        msg_Err( p_filter, "cannot spawn vnc update request thread" );
        goto exit;
    }
    /* connection is initialized, now read and handle server messages */
    vlc_restorecancel (canc);
    for( ;; )
    {
        rfbServerToClientMsg msg;
        int i_msgSize;
        memset( &msg, 0, sizeof(msg) );
        if( !read_exact(p_filter, p_sys->i_socket, (char*)&msg, 1 ) )
        {
            msg_Err( p_filter, "Error while waiting for next server message");
            break;
        }
        switch (msg.type)
        {
        case rfbFramebufferUpdate:
            i_msgSize = sz_rfbFramebufferUpdateMsg;
            break;
        case rfbSetColourMapEntries:
            i_msgSize = sz_rfbSetColourMapEntriesMsg;
            break;
        case rfbBell:
            i_msgSize = sz_rfbBellMsg;
            break;
        case rfbServerCutText:
            i_msgSize = sz_rfbServerCutTextMsg;
            break;
        case rfbReSizeFrameBuffer:
            i_msgSize = sz_rfbReSizeFrameBufferMsg;
            break;
        default:
            i_msgSize = 0;
            msg_Err( p_filter, "Invalid message %u received", msg.type );
            break;
        }
        if( i_msgSize <= 0 )
            break;
        if( --i_msgSize > 0 )
        {
            if ( !read_exact( p_filter, p_sys->i_socket,
                              ((char*)&msg)+1, i_msgSize ) )
            {
                msg_Err( p_filter, "Error while reading message of type %u",
                         msg.type );
                break;
            }
        }
        canc = vlc_savecancel ();
        process_server_message( p_filter, &msg);
        vlc_restorecancel (canc);
    }
    canc = vlc_savecancel ();
    msg_Dbg( p_filter, "joining update_request_thread" );
    vlc_cancel( update_request_thread_handle );
    vlc_join( update_request_thread_handle, NULL );
    msg_Dbg( p_filter, "released update_request_thread" );
exit:
    vlc_mutex_lock( &p_sys->lock );
    p_sys->b_connection_active = false;
    if (p_sys->i_socket >= 0)
        net_Close(p_sys->i_socket);
    if( p_sys->p_pic )
        picture_Release( p_sys->p_pic );
    /* It will hide the subtitle */
    p_sys->b_continue = false;
    p_sys->b_need_update = true;
    vlc_mutex_unlock( &p_sys->lock );
    msg_Dbg( p_filter, "VNC message reader thread ended" );
    vlc_restorecancel (canc);
    return NULL;
}
