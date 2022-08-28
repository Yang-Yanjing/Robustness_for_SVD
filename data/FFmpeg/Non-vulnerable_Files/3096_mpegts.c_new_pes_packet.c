static void new_pes_packet(PESContext *pes, AVPacket *pkt)
{
    av_init_packet(pkt);
    pkt->buf  = pes->buffer;
    pkt->data = pes->buffer->data;
    pkt->size = pes->data_index;
    if (pes->total_size != MAX_PES_PAYLOAD &&
        pes->pes_header_size + pes->data_index != pes->total_size +
        PES_START_SIZE) {
        av_log(pes->stream, AV_LOG_WARNING, "PES packet size mismatch\n");
        pes->flags |= AV_PKT_FLAG_CORRUPT;
    }
    memset(pkt->data + pkt->size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    
    if (pes->sub_st && pes->stream_type == 0x83 && pes->extended_stream_id == 0x76)
        pkt->stream_index = pes->sub_st->index;
    else
        pkt->stream_index = pes->st->index;
    pkt->pts = pes->pts;
    pkt->dts = pes->dts;
    
    pkt->pos   = pes->ts_packet_pos;
    pkt->flags = pes->flags;
    pes->buffer = NULL;
    reset_pes_packet_state(pes);
}
