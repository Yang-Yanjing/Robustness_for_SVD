static int find_header_idx(AVFormatContext *s, AVCodecContext *c, int size, int frame_type)
{
    NUTContext *nut = s->priv_data;
    uint8_t out[64];
    int i;
    int len = find_expected_header(c, size, frame_type, out);
    for (i = 1; i < nut->header_count; i++) {
        if (len == nut->header_len[i] && !memcmp(out, nut->header[i], len)) {
            return i;
        }
    }
    return 0;
}
