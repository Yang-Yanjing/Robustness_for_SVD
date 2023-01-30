}
static void InitStatistics( input_thread_t * p_input )
{
    if( p_input->b_preparsing ) return;
    /* Prepare statistics */
#define INIT_COUNTER( c, compute ) p_input->p->counters.p_##c = \
 stats_CounterCreate( STATS_##compute);
    if( libvlc_stats( p_input ) )
    {
        INIT_COUNTER( read_bytes, COUNTER );
        INIT_COUNTER( read_packets, COUNTER );
        INIT_COUNTER( demux_read, COUNTER );
        INIT_COUNTER( input_bitrate, DERIVATIVE );
        INIT_COUNTER( demux_bitrate, DERIVATIVE );
        INIT_COUNTER( demux_corrupted, COUNTER );
        INIT_COUNTER( demux_discontinuity, COUNTER );
        INIT_COUNTER( played_abuffers, COUNTER );
        INIT_COUNTER( lost_abuffers, COUNTER );
        INIT_COUNTER( displayed_pictures, COUNTER );
        INIT_COUNTER( lost_pictures, COUNTER );
        INIT_COUNTER( decoded_audio, COUNTER );
        INIT_COUNTER( decoded_video, COUNTER );
        INIT_COUNTER( decoded_sub, COUNTER );
        p_input->p->counters.p_sout_send_bitrate = NULL;
        p_input->p->counters.p_sout_sent_packets = NULL;
        p_input->p->counters.p_sout_sent_bytes = NULL;
    }
}
