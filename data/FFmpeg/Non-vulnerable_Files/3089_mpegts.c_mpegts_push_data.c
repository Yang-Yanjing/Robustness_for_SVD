static int mpegts_push_data(MpegTSFilter *filter,
                            const uint8_t *buf, int buf_size, int is_start,
                            int64_t pos)
{
    PESContext *pes   = filter->u.pes_filter.opaque;
    MpegTSContext *ts = pes->ts;
    const uint8_t *p;
    int len, code;
    if (!ts->pkt)
        return 0;
    if (is_start) {
        if (pes->state == MPEGTS_PAYLOAD && pes->data_index > 0) {
            new_pes_packet(pes, ts->pkt);
            ts->stop_parse = 1;
        } else {
            reset_pes_packet_state(pes);
        }
        pes->state         = MPEGTS_HEADER;
        pes->ts_packet_pos = pos;
    }
    p = buf;
    while (buf_size > 0) {
        switch (pes->state) {
        case MPEGTS_HEADER:
            len = PES_START_SIZE - pes->data_index;
            if (len > buf_size)
                len = buf_size;
            memcpy(pes->header + pes->data_index, p, len);
            pes->data_index += len;
            p += len;
            buf_size -= len;
            if (pes->data_index == PES_START_SIZE) {
                

                if (pes->header[0] == 0x00 && pes->header[1] == 0x00 &&
                    pes->header[2] == 0x01) {
                    
                    code = pes->header[3] | 0x100;
                    av_log(pes->stream, AV_LOG_TRACE, "pid=%x pes_code=%#x\n", pes->pid,
                            code);
                    if ((pes->st && pes->st->discard == AVDISCARD_ALL &&
                         (!pes->sub_st ||
                          pes->sub_st->discard == AVDISCARD_ALL)) ||
                        code == 0x1be) 
                        goto skip;
                    
                    if (!pes->st) {
                        if (ts->skip_changes)
                            goto skip;
                        pes->st = avformat_new_stream(ts->stream, NULL);
                        if (!pes->st)
                            return AVERROR(ENOMEM);
                        pes->st->id = pes->pid;
                        mpegts_set_stream_info(pes->st, pes, 0, 0);
                    }
                    pes->total_size = AV_RB16(pes->header + 4);
                    

                    if (!pes->total_size)
                        pes->total_size = MAX_PES_PAYLOAD;
                    
                    pes->buffer = av_buffer_alloc(pes->total_size +
                                                  AV_INPUT_BUFFER_PADDING_SIZE);
                    if (!pes->buffer)
                        return AVERROR(ENOMEM);
                    if (code != 0x1bc && code != 0x1bf && 
                        code != 0x1f0 && code != 0x1f1 && 
                        code != 0x1ff && code != 0x1f2 && 
                        code != 0x1f8) {                  
                        pes->state = MPEGTS_PESHEADER;
                        if (pes->st->codec->codec_id == AV_CODEC_ID_NONE && !pes->st->request_probe) {
                            av_log(pes->stream, AV_LOG_TRACE,
                                    "pid=%x stream_type=%x probing\n",
                                    pes->pid,
                                    pes->stream_type);
                            pes->st->request_probe = 1;
                        }
                    } else {
                        pes->pes_header_size = 6;
                        pes->state      = MPEGTS_PAYLOAD;
                        pes->data_index = 0;
                    }
                } else {
                    
                    
skip:
                    pes->state = MPEGTS_SKIP;
                    continue;
                }
            }
            break;
        
        
        case MPEGTS_PESHEADER:
            len = PES_HEADER_SIZE - pes->data_index;
            if (len < 0)
                return AVERROR_INVALIDDATA;
            if (len > buf_size)
                len = buf_size;
            memcpy(pes->header + pes->data_index, p, len);
            pes->data_index += len;
            p += len;
            buf_size -= len;
            if (pes->data_index == PES_HEADER_SIZE) {
                pes->pes_header_size = pes->header[8] + 9;
                pes->state           = MPEGTS_PESHEADER_FILL;
            }
            break;
        case MPEGTS_PESHEADER_FILL:
            len = pes->pes_header_size - pes->data_index;
            if (len < 0)
                return AVERROR_INVALIDDATA;
            if (len > buf_size)
                len = buf_size;
            memcpy(pes->header + pes->data_index, p, len);
            pes->data_index += len;
            p += len;
            buf_size -= len;
            if (pes->data_index == pes->pes_header_size) {
                const uint8_t *r;
                unsigned int flags, pes_ext, skip;
                flags = pes->header[7];
                r = pes->header + 9;
                pes->pts = AV_NOPTS_VALUE;
                pes->dts = AV_NOPTS_VALUE;
                if ((flags & 0xc0) == 0x80) {
                    pes->dts = pes->pts = ff_parse_pes_pts(r);
                    r += 5;
                } else if ((flags & 0xc0) == 0xc0) {
                    pes->pts = ff_parse_pes_pts(r);
                    r += 5;
                    pes->dts = ff_parse_pes_pts(r);
                    r += 5;
                }
                pes->extended_stream_id = -1;
                if (flags & 0x01) { 
                    pes_ext = *r++;
                    
                    skip  = (pes_ext >> 4) & 0xb;
                    skip += skip & 0x9;
                    r    += skip;
                    if ((pes_ext & 0x41) == 0x01 &&
                        (r + 2) <= (pes->header + pes->pes_header_size)) {
                        
                        if ((r[0] & 0x7f) > 0 && (r[1] & 0x80) == 0)
                            pes->extended_stream_id = r[1];
                    }
                }
                
                pes->state = MPEGTS_PAYLOAD;
                pes->data_index = 0;
                if (pes->stream_type == 0x12 && buf_size > 0) {
                    int sl_header_bytes = read_sl_header(pes, &pes->sl, p,
                                                         buf_size);
                    pes->pes_header_size += sl_header_bytes;
                    p += sl_header_bytes;
                    buf_size -= sl_header_bytes;
                }
                if (pes->stream_type == 0x15 && buf_size >= 5) {
                    
                    pes->pes_header_size += 5;
                    p += 5;
                    buf_size -= 5;
                }
                if (   pes->ts->fix_teletext_pts
                    && (   pes->st->codec->codec_id == AV_CODEC_ID_DVB_TELETEXT
                        || pes->st->codec->codec_id == AV_CODEC_ID_DVB_SUBTITLE)
                    ) {
                    AVProgram *p = NULL;
                    while ((p = av_find_program_from_stream(pes->stream, p, pes->st->index))) {
                        if (p->pcr_pid != -1 && p->discard != AVDISCARD_ALL) {
                            MpegTSFilter *f = pes->ts->pids[p->pcr_pid];
                            if (f) {
                                AVStream *st = NULL;
                                if (f->type == MPEGTS_PES) {
                                    PESContext *pcrpes = f->u.pes_filter.opaque;
                                    if (pcrpes)
                                        st = pcrpes->st;
                                } else if (f->type == MPEGTS_PCR) {
                                    int i;
                                    for (i = 0; i < p->nb_stream_indexes; i++) {
                                        AVStream *pst = pes->stream->streams[p->stream_index[i]];
                                        if (pst->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                                            st = pst;
                                    }
                                }
                                if (f->last_pcr != -1 && st && st->discard != AVDISCARD_ALL) {
                                    
                                    
                                    
                                    
                                    int64_t pcr = f->last_pcr / 300;
                                    pes->st->pts_wrap_reference = st->pts_wrap_reference;
                                    pes->st->pts_wrap_behavior = st->pts_wrap_behavior;
                                    if (pes->dts == AV_NOPTS_VALUE || pes->dts < pcr) {
                                        pes->pts = pes->dts = pcr;
                                    } else if (pes->st->codec->codec_id == AV_CODEC_ID_DVB_TELETEXT &&
                                               pes->dts > pcr + 3654 + 9000) {
                                        pes->pts = pes->dts = pcr + 3654 + 9000;
                                    } else if (pes->st->codec->codec_id == AV_CODEC_ID_DVB_SUBTITLE &&
                                               pes->dts > pcr + 10*90000) { 
                                        pes->pts = pes->dts = pcr + 3654 + 9000;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case MPEGTS_PAYLOAD:
            if (pes->buffer) {
                if (pes->data_index > 0 &&
                    pes->data_index + buf_size > pes->total_size) {
                    new_pes_packet(pes, ts->pkt);
                    pes->total_size = MAX_PES_PAYLOAD;
                    pes->buffer = av_buffer_alloc(pes->total_size +
                                                  AV_INPUT_BUFFER_PADDING_SIZE);
                    if (!pes->buffer)
                        return AVERROR(ENOMEM);
                    ts->stop_parse = 1;
                } else if (pes->data_index == 0 &&
                           buf_size > pes->total_size) {
                    
                    
                    buf_size = pes->total_size;
                }
                memcpy(pes->buffer->data + pes->data_index, p, buf_size);
                pes->data_index += buf_size;
                




                if (!ts->stop_parse && pes->total_size < MAX_PES_PAYLOAD &&
                    pes->pes_header_size + pes->data_index == pes->total_size + PES_START_SIZE) {
                    ts->stop_parse = 1;
                    new_pes_packet(pes, ts->pkt);
                }
            }
            buf_size = 0;
            break;
        case MPEGTS_SKIP:
            buf_size = 0;
            break;
        }
    }
    return 0;
}
