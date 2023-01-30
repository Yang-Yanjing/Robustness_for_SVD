}
static bool OggSeekToPacket( demux_t *p_demux, logical_stream_t *p_stream,
            int64_t i_granulepos, packetStartCoordinates *p_lastpacketcoords,
            bool b_exact )
{
    ogg_packet op;
    demux_sys_t *p_sys  = p_demux->p_sys;
    if ( ogg_stream_pagein( &p_stream->os, &p_sys->current_page ) != 0 )
        return false;
    p_sys->b_page_waiting = true;
    int i=0;
    int64_t itarget_frame = Ogg_GetKeyframeGranule( p_stream, i_granulepos );
    int64_t iframe = Ogg_GetKeyframeGranule( p_stream, ogg_page_granulepos( &p_sys->current_page ) );
    if ( ! ogg_page_continued( &p_sys->current_page ) )
    {
        /* Start of frame, not continued page, but no packet. */
        p_lastpacketcoords->i_pageno = ogg_page_pageno( &p_sys->current_page );
        p_lastpacketcoords->i_pos = p_sys->i_input_position;
        p_lastpacketcoords->i_skip = 0;
    }
    if ( b_exact && iframe > itarget_frame )
    {
        while( ogg_stream_packetout( &p_stream->os, &op ) > 0 ) {};
        p_sys->b_page_waiting = false;
        return false;
    }
    while( ogg_stream_packetpeek( &p_stream->os, &op ) > 0 )
    {
        if ( ( !b_exact || itarget_frame == iframe ) && Ogg_IsKeyFrame( p_stream, &op ) )
        {
            OggDebug(
                msg_Dbg(p_demux, "** KEYFRAME **" );
                msg_Dbg(p_demux, "** KEYFRAME PACKET START pageno %"PRId64" OFFSET %"PRId64" skip %"PRId64" **", p_lastpacketcoords->i_pageno, p_lastpacketcoords->i_pos, p_lastpacketcoords->i_skip );
                msg_Dbg(p_demux, "KEYFRAME PACKET IS at pageno %"PRId64" OFFSET %"PRId64" with skip %d packet (%d / %d) ",
                    ogg_page_pageno( &p_sys->current_page ), p_sys->i_input_position, i, i+1, ogg_page_packets( &p_sys->current_page ) );
                DemuxDebug( p_sys->b_seeked = true; )
            );
            if ( i != 0 ) /* Not continued packet */
            {
                /* We need to handle the case when the packet spans onto N
                       previous page(s). packetout() will be valid only when
                       all segments are assembled.
                       Keyframe flag is only available after assembling last part
                       (when packetout() becomes valid). We have no way to guess
                       keyframe at earlier time.
                    */
                p_lastpacketcoords->i_pageno = ogg_page_pageno( &p_sys->current_page );
                p_lastpacketcoords->i_pos = p_sys->i_input_position;
                p_lastpacketcoords->i_skip = i;
            }
            return true;
        }
        p_lastpacketcoords->i_pageno = ogg_page_pageno( &p_sys->current_page );
        p_lastpacketcoords->i_pos = p_sys->i_input_position;
        p_lastpacketcoords->i_skip = i + 1;
        i++;
        /* remove that packet and go sync to next */
        ogg_stream_packetout( &p_stream->os, &op );
    }
    return false;
}
