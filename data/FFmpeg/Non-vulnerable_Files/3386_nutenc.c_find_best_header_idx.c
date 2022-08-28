static int find_best_header_idx(NUTContext *nut, AVPacket *pkt)
{
    int i;
    int best_i   = 0;
    int best_len = 0;
    if (pkt->size > 4096)
        return 0;
    for (i = 1; i < nut->header_count; i++)
        if (pkt->size >= nut->header_len[i]
            && nut->header_len[i] > best_len
            && !memcmp(pkt->data, nut->header[i], nut->header_len[i])) {
            best_i   = i;
            best_len = nut->header_len[i];
        }
    return best_i;
}
