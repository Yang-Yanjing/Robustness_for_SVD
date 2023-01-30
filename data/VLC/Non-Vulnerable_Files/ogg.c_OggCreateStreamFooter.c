}
static void OggCreateStreamFooter( sout_mux_t *p_mux, ogg_stream_t *p_stream )
{
    block_t *p_og;
    ogg_packet op;
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    /* as stream is finished, overwrite the index, if there was any */
    if ( p_sys->skeleton.b_create && p_stream->skeleton.p_index
         && p_stream->skeleton.i_index_payload )
    {
        sout_AccessOutSeek( p_mux->p_access, p_stream->skeleton.i_index_offset );
        OggGetSkeletonIndex( &op.packet, &op.bytes, p_stream );
        if ( op.packet != NULL )
        {
            msg_Dbg(p_mux, "Rewriting index at %"PRId64, p_stream->skeleton.i_index_offset );
            ogg_stream_reset_serialno( &p_sys->skeleton.os, p_sys->skeleton.i_serial_no );
            op.b_o_s = 0;
            op.e_o_s = 0;
            op.granulepos = 0;
            op.packetno = p_stream->skeleton.i_index_packetno + 1;
            /* fake our stream state */
            p_sys->skeleton.os.pageno = p_stream->skeleton.i_index_pageno;
            p_sys->skeleton.os.packetno = p_stream->skeleton.i_index_packetno;
            p_sys->skeleton.os.granulepos = 0;
            p_sys->skeleton.os.b_o_s = 1;
            p_sys->skeleton.os.e_o_s = 0;
            ogg_stream_packetin( &p_sys->skeleton.os, &op );
            p_og = OggStreamFlush( p_mux, &p_sys->skeleton.os, 0 );
            sout_AccessOutWrite( p_mux->p_access, p_og );
        }
        sout_AccessOutSeek( p_mux->p_access, p_sys->i_pos );
    }
    /* clear skeleton */
    p_stream->skeleton.b_fisbone_done = false;
    p_stream->skeleton.b_index_done = false;
    p_stream->skeleton.i_index_offset = 0;
    p_stream->skeleton.i_index_payload = 0;
    p_stream->skeleton.i_last_keyframe_pos = 0;
    p_stream->skeleton.i_last_keyframe_time = 0;
    /* clear accounting */
    p_stream->i_num_frames = 0;
    p_stream->i_num_keyframes = 0;
    /* Write eos packet for stream. */
    op.packet = NULL;
    op.bytes  = 0;
    op.b_o_s  = 0;
    op.e_o_s  = 1;
    op.granulepos = p_stream->u_last_granulepos;
    op.packetno = p_stream->i_packet_no++;
    ogg_stream_packetin( &p_stream->os, &op );
    /* flush it with all remaining data */
    if( ( p_og = OggStreamFlush( p_mux, &p_stream->os, 0 ) ) )
    {
        /* Write footer */
        OggSetDate( p_og, p_stream->i_dts, p_stream->i_length );
        p_mux->p_sys->i_pos += sout_AccessOutWrite( p_mux->p_access, p_og );
    }
    ogg_stream_clear( &p_stream->os );
}
