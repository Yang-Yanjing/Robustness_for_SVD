 *****************************************************************************/
void input_UpdateStatistic( input_thread_t *p_input,
                            input_statistic_t i_type, int i_delta )
{
    assert( p_input->p->i_state != INIT_S );
    vlc_mutex_lock( &p_input->p->counters.counters_lock);
    switch( i_type )
    {
#define I(c) stats_Update( p_input->p->counters.c, i_delta, NULL )
    case INPUT_STATISTIC_DECODED_VIDEO:
        I(p_decoded_video);
        break;
    case INPUT_STATISTIC_DECODED_AUDIO:
        I(p_decoded_audio);
        break;
    case INPUT_STATISTIC_DECODED_SUBTITLE:
        I(p_decoded_sub);
        break;
    case INPUT_STATISTIC_SENT_PACKET:
        I(p_sout_sent_packets);
        break;
#undef I
    case INPUT_STATISTIC_SENT_BYTE:
    {
        uint64_t bytes;
        stats_Update( p_input->p->counters.p_sout_sent_bytes, i_delta, &bytes );
        stats_Update( p_input->p->counters.p_sout_send_bitrate, bytes, NULL );
        break;
    }
    default:
        msg_Err( p_input, "Invalid statistic type %d (internal error)", i_type );
        break;
    }
    vlc_mutex_unlock( &p_input->p->counters.counters_lock);
}
