static int audio_write_packet(AVFormatContext *s1, AVPacket *pkt)
{
    SndioData *s = s1->priv_data;
    uint8_t *buf= pkt->data;
    int size = pkt->size;
    int len, ret;
    while (size > 0) {
        len = FFMIN(s->buffer_size - s->buffer_offset, size);
        memcpy(s->buffer + s->buffer_offset, buf, len);
        buf  += len;
        size -= len;
        s->buffer_offset += len;
        if (s->buffer_offset >= s->buffer_size) {
            ret = sio_write(s->hdl, s->buffer, s->buffer_size);
            if (ret == 0 || sio_eof(s->hdl))
                return AVERROR(EIO);
            s->softpos      += ret;
            s->buffer_offset = 0;
        }
    }
    return 0;
}
