static int get_needed_flags(NUTContext *nut, StreamContext *nus, FrameCode *fc,
                            AVPacket *pkt)
{
    int flags = 0;
    if (pkt->flags & AV_PKT_FLAG_KEY)
        flags |= FLAG_KEY;
    if (pkt->stream_index != fc->stream_id)
        flags |= FLAG_STREAM_ID;
    if (pkt->size / fc->size_mul)
        flags |= FLAG_SIZE_MSB;
    if (pkt->pts - nus->last_pts != fc->pts_delta)
        flags |= FLAG_CODED_PTS;
    if (pkt->side_data_elems && nut->version > 3)
        flags |= FLAG_SM_DATA;
    if (pkt->size > 2 * nut->max_distance)
        flags |= FLAG_CHECKSUM;
    if (FFABS(pkt->pts - nus->last_pts) > nus->max_pts_distance)
        flags |= FLAG_CHECKSUM;
    if (pkt->size < nut->header_len[fc->header_idx] ||
        (pkt->size > 4096 && fc->header_idx)        ||
        memcmp(pkt->data, nut->header[fc->header_idx],
               nut->header_len[fc->header_idx]))
        flags |= FLAG_HEADER_IDX;
    return flags | (fc->flags & FLAG_CODED);
}
