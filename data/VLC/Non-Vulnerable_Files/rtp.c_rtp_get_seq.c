}
uint16_t rtp_get_seq( sout_stream_id_sys_t *id )
{
    /* This will return values for the next packet. */
    uint16_t seq;
    vlc_mutex_lock( &id->lock_sink );
    seq = id->i_seq_sent_next;
    vlc_mutex_unlock( &id->lock_sink );
    return seq;
}
