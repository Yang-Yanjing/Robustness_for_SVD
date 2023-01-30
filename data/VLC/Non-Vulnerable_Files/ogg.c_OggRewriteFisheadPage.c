}
static void OggRewriteFisheadPage( sout_mux_t *p_mux )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    ogg_packet op;
    op.bytes = 80;
    op.packet = calloc( 1, op.bytes );
    if ( op.packet != NULL )
    {
        op.b_o_s = 1;
        op.e_o_s = 0;
        op.granulepos = 0;
        op.packetno = 0;
        ogg_stream_reset_serialno( &p_sys->skeleton.os, p_sys->skeleton.i_serial_no );
        OggFillSkeletonFishead( op.packet, p_mux );
        ogg_stream_packetin( &p_sys->skeleton.os, &op );
        msg_Dbg( p_mux, "rewriting fishead at %"PRId64, p_mux->p_sys->skeleton.i_fishead_offset );
        sout_AccessOutSeek( p_mux->p_access, p_mux->p_sys->skeleton.i_fishead_offset );
        sout_AccessOutWrite( p_mux->p_access,
                             OggStreamFlush( p_mux, &p_sys->skeleton.os, 0 ) );
        sout_AccessOutSeek( p_mux->p_access, p_mux->p_sys->i_pos );
    }
}
