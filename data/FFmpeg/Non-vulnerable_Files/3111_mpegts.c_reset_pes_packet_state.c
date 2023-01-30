static void reset_pes_packet_state(PESContext *pes)
{
    pes->pts        = AV_NOPTS_VALUE;
    pes->dts        = AV_NOPTS_VALUE;
    pes->data_index = 0;
    pes->flags      = 0;
    av_buffer_unref(&pes->buffer);
}
