static int output_packet(AVFormatContext *ctx, int flush)
{
    MpegMuxContext *s = ctx->priv_data;
    AVStream *st;
    StreamInfo *stream;
    int i, avail_space = 0, es_size, trailer_size;
    int best_i = -1;
    int best_score = INT_MIN;
    int ignore_constraints = 0;
    int ignore_delay = 0;
    int64_t scr = s->last_scr;
    PacketDesc *timestamp_packet;
    const int64_t max_delay = av_rescale(ctx->max_delay, 90000, AV_TIME_BASE);
retry:
    for (i = 0; i < ctx->nb_streams; i++) {
        AVStream *st = ctx->streams[i];
        StreamInfo *stream = st->priv_data;
        const int avail_data = av_fifo_size(stream->fifo);
        const int space = stream->max_buffer_size - stream->buffer_index;
        int rel_space = 1024LL * space / stream->max_buffer_size;
        PacketDesc *next_pkt = stream->premux_packet;
        

        if (s->packet_size > avail_data && !flush
            && st->codec->codec_type != AVMEDIA_TYPE_SUBTITLE)
            return 0;
        if (avail_data == 0)
            continue;
        av_assert0(avail_data > 0);
        if (space < s->packet_size && !ignore_constraints)
            continue;
        if (next_pkt && next_pkt->dts - scr > max_delay && !ignore_delay)
            continue;
        if (   stream->predecode_packet
            && stream->predecode_packet->size > stream->buffer_index)
            rel_space += 1<<28;
        if (rel_space > best_score) {
            best_score  = rel_space;
            best_i      = i;
            avail_space = space;
        }
    }
    if (best_i < 0) {
        int64_t best_dts = INT64_MAX;
        int has_premux = 0;
        for (i = 0; i < ctx->nb_streams; i++) {
            AVStream *st = ctx->streams[i];
            StreamInfo *stream = st->priv_data;
            PacketDesc *pkt_desc = stream->predecode_packet;
            if (pkt_desc && pkt_desc->dts < best_dts)
                best_dts = pkt_desc->dts;
            has_premux |= !!stream->premux_packet;
        }
        if (best_dts < INT64_MAX) {
            av_log(ctx, AV_LOG_TRACE, "bumping scr, scr:%f, dts:%f\n",
                    scr / 90000.0, best_dts / 90000.0);
            if (scr >= best_dts + 1 && !ignore_constraints) {
                av_log(ctx, AV_LOG_ERROR,
                    "packet too large, ignoring buffer limits to mux it\n");
                ignore_constraints = 1;
            }
            scr = FFMAX(best_dts + 1, scr);
            if (remove_decoded_packets(ctx, scr) < 0)
                return -1;
        } else if (has_premux && flush) {
            av_log(ctx, AV_LOG_ERROR,
                  "delay too large, ignoring ...\n");
            ignore_delay = 1;
            ignore_constraints = 1;
        } else
            return 0;
        goto retry;
    }
    av_assert0(best_i >= 0);
    st     = ctx->streams[best_i];
    stream = st->priv_data;
    av_assert0(av_fifo_size(stream->fifo) > 0);
    av_assert0(avail_space >= s->packet_size || ignore_constraints);
    timestamp_packet = stream->premux_packet;
    if (timestamp_packet->unwritten_size == timestamp_packet->size) {
        trailer_size = 0;
    } else {
        trailer_size     = timestamp_packet->unwritten_size;
        timestamp_packet = timestamp_packet->next;
    }
    if (timestamp_packet) {
        av_log(ctx, AV_LOG_TRACE, "dts:%f pts:%f scr:%f stream:%d\n",
                timestamp_packet->dts / 90000.0,
                timestamp_packet->pts / 90000.0,
                scr / 90000.0, best_i);
        es_size = flush_packet(ctx, best_i, timestamp_packet->pts,
                               timestamp_packet->dts, scr, trailer_size);
    } else {
        av_assert0(av_fifo_size(stream->fifo) == trailer_size);
        es_size = flush_packet(ctx, best_i, AV_NOPTS_VALUE, AV_NOPTS_VALUE, scr,
                               trailer_size);
    }
    if (s->is_vcd) {
        

        int vcd_pad_bytes;
        
        while ((vcd_pad_bytes = get_vcd_padding_size(ctx, stream->premux_packet->pts)) >= s->packet_size) {
            put_vcd_padding_sector(ctx);
            
            s->last_scr += s->packet_size * 90000LL / (s->mux_rate * 50LL);
        }
    }
    stream->buffer_index += es_size;
    
    s->last_scr          += s->packet_size * 90000LL / (s->mux_rate * 50LL);
    while (stream->premux_packet &&
           stream->premux_packet->unwritten_size <= es_size) {
        es_size              -= stream->premux_packet->unwritten_size;
        stream->premux_packet = stream->premux_packet->next;
    }
    if (es_size) {
        av_assert0(stream->premux_packet);
        stream->premux_packet->unwritten_size -= es_size;
    }
    if (remove_decoded_packets(ctx, s->last_scr) < 0)
        return -1;
    return 1;
}
