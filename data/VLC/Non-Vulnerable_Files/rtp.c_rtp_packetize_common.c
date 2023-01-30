}
void rtp_packetize_common( sout_stream_id_sys_t *id, block_t *out,
                           int b_marker, int64_t i_pts )
{
    if( !id->b_ts_init )
    {
        sout_stream_sys_t *p_sys = id->p_stream->p_sys;
        vlc_mutex_lock( &p_sys->lock_ts );
        if( p_sys->i_npt_zero == VLC_TS_INVALID )
        {
            /* This is the first packet of any ES. We initialize the
             * NPT=0 time reference, and the offset to match the
             * arbitrary PTS reference. */
            p_sys->i_npt_zero = i_pts + id->i_caching;
            p_sys->i_pts_offset = p_sys->i_pts_zero - i_pts;
        }
        vlc_mutex_unlock( &p_sys->lock_ts );
        /* And in any case this is the first packet of this ES, so we
         * initialize the offset for this ES. */
        id->i_ts_offset = rtp_compute_ts( id->rtp_fmt.clock_rate,
                                          p_sys->i_pts_offset );
        id->b_ts_init = true;
    }
    uint32_t i_timestamp = rtp_compute_ts( id->rtp_fmt.clock_rate, i_pts )
                           + id->i_ts_offset;
    out->p_buffer[0] = 0x80;
    out->p_buffer[1] = (b_marker?0x80:0x00)|id->rtp_fmt.payload_type;
    out->p_buffer[2] = ( id->i_sequence >> 8)&0xff;
    out->p_buffer[3] = ( id->i_sequence     )&0xff;
    out->p_buffer[4] = ( i_timestamp >> 24 )&0xff;
    out->p_buffer[5] = ( i_timestamp >> 16 )&0xff;
    out->p_buffer[6] = ( i_timestamp >>  8 )&0xff;
    out->p_buffer[7] = ( i_timestamp       )&0xff;
    memcpy( out->p_buffer + 8, id->ssrc, 4 );
    out->i_buffer = 12;
    id->i_sequence++;
}
