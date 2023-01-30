static int rtmp_read(URLContext *s, uint8_t *buf, int size)
{
    RTMPContext *rt = s->priv_data;
    int orig_size = size;
    int ret;
    while (size > 0) {
        int data_left = rt->flv_size - rt->flv_off;
        if (data_left >= size) {
            memcpy(buf, rt->flv_data + rt->flv_off, size);
            rt->flv_off += size;
            return orig_size;
        }
        if (data_left > 0) {
            memcpy(buf, rt->flv_data + rt->flv_off, data_left);
            buf  += data_left;
            size -= data_left;
            rt->flv_off = rt->flv_size;
            return data_left;
        }
        if ((ret = get_packet(s, 0)) < 0)
           return ret;
    }
    return orig_size;
}
