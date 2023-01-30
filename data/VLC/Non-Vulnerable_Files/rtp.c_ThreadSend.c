 ****************************************************************************/
static void* ThreadSend( void *data )
{
#ifdef _WIN32
# define ENOBUFS      WSAENOBUFS
# define EAGAIN       WSAEWOULDBLOCK
# define EWOULDBLOCK  WSAEWOULDBLOCK
#endif
    sout_stream_id_sys_t *id = data;
    unsigned i_caching = id->i_caching;
    for (;;)
    {
        block_t *out = block_FifoGet( id->p_fifo );
        block_cleanup_push (out);
#ifdef HAVE_SRTP
        if( id->srtp )
        {   /* FIXME: this is awfully inefficient */
            size_t len = out->i_buffer;
            out = block_Realloc( out, 0, len + 10 );
            out->i_buffer = len;
            int canc = vlc_savecancel ();
            int val = srtp_send( id->srtp, out->p_buffer, &len, len + 10 );
            vlc_restorecancel (canc);
            if( val )
            {
                msg_Dbg( id->p_stream, "SRTP sending error: %s",
                         vlc_strerror_c(val) );
                block_Release( out );
                out = NULL;
            }
            else
                out->i_buffer = len;
        }
        if (out)
            mwait (out->i_dts + i_caching);
        vlc_cleanup_pop ();
        if (out == NULL)
            continue;
#else
        mwait (out->i_dts + i_caching);
        vlc_cleanup_pop ();
#endif
        ssize_t len = out->i_buffer;
        int canc = vlc_savecancel ();
        vlc_mutex_lock( &id->lock_sink );
        unsigned deadc = 0; /* How many dead sockets? */
        int deadv[id->sinkc]; /* Dead sockets list */
        for( int i = 0; i < id->sinkc; i++ )
        {
#ifdef HAVE_SRTP
            if( !id->srtp ) /* FIXME: SRTCP support */
#endif
                SendRTCP( id->sinkv[i].rtcp, out );
            if( send( id->sinkv[i].rtp_fd, out->p_buffer, len, 0 ) == -1
             && net_errno != EAGAIN && net_errno != EWOULDBLOCK
             && net_errno != ENOBUFS && net_errno != ENOMEM )
            {
                int type;
                getsockopt( id->sinkv[i].rtp_fd, SOL_SOCKET, SO_TYPE,
                            &type, &(socklen_t){ sizeof(type) });
                if( type == SOCK_DGRAM )
                    /* ICMP soft error: ignore and retry */
                    send( id->sinkv[i].rtp_fd, out->p_buffer, len, 0 );
                else
                    /* Broken connection */
                    deadv[deadc++] = id->sinkv[i].rtp_fd;
            }
        }
        id->i_seq_sent_next = ntohs(((uint16_t *) out->p_buffer)[1]) + 1;
        vlc_mutex_unlock( &id->lock_sink );
        block_Release( out );
        for( unsigned i = 0; i < deadc; i++ )
        {
            msg_Dbg( id->p_stream, "removing socket %d", deadv[i] );
            rtp_del_sink( id, deadv[i] );
        }
        vlc_restorecancel (canc);
    }
    return NULL;
}
