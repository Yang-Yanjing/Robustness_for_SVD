static int nut_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    NUTContext *nut = s->priv_data;
    AVIOContext *bc = s->pb;
    int i, frame_code = 0, ret, skip;
    int64_t ts, back_ptr;
    for (;;) {
        int64_t pos  = avio_tell(bc);
        uint64_t tmp = nut->next_startcode;
        nut->next_startcode = 0;
        if (tmp) {
            pos -= 8;
        } else {
            frame_code = avio_r8(bc);
            if (avio_feof(bc))
                return AVERROR_EOF;
            if (frame_code == 'N') {
                tmp = frame_code;
                for (i = 1; i < 8; i++)
                    tmp = (tmp << 8) + avio_r8(bc);
            }
        }
        switch (tmp) {
        case MAIN_STARTCODE:
        case STREAM_STARTCODE:
        case INDEX_STARTCODE:
            skip = get_packetheader(nut, bc, 0, tmp);
            avio_skip(bc, skip);
            break;
        case INFO_STARTCODE:
            if (decode_info_header(nut) < 0)
                goto resync;
            break;
        case SYNCPOINT_STARTCODE:
            if (decode_syncpoint(nut, &ts, &back_ptr) < 0)
                goto resync;
            frame_code = avio_r8(bc);
        case 0:
            ret = decode_frame(nut, pkt, frame_code);
            if (ret == 0)
                return 0;
            else if (ret == 1) 
                break;
        default:
resync:
            av_log(s, AV_LOG_DEBUG, "syncing from %"PRId64"\n", pos);
            tmp = find_any_startcode(bc, FFMAX(nut->last_syncpoint_pos, nut->last_resync_pos) + 1);
            nut->last_resync_pos = avio_tell(bc);
            if (tmp == 0)
                return AVERROR_INVALIDDATA;
            av_log(s, AV_LOG_DEBUG, "sync\n");
            nut->next_startcode = tmp;
        }
    }
}
