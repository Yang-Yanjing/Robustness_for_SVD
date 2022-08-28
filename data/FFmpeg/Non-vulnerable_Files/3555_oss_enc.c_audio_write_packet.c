static int audio_write_packet(AVFormatContext *s1, AVPacket *pkt)
{
    OSSAudioData *s = s1->priv_data;
    int len, ret;
    int size= pkt->size;
    uint8_t *buf= pkt->data;
    while (size > 0) {
        len = FFMIN(OSS_AUDIO_BLOCK_SIZE - s->buffer_ptr, size);
        memcpy(s->buffer + s->buffer_ptr, buf, len);
        s->buffer_ptr += len;
        if (s->buffer_ptr >= OSS_AUDIO_BLOCK_SIZE) {
            for(;;) {
                ret = write(s->fd, s->buffer, OSS_AUDIO_BLOCK_SIZE);
                if (ret > 0)
                    break;
                if (ret < 0 && (errno != EAGAIN && errno != EINTR))
                    return AVERROR(EIO);
            }
            s->buffer_ptr = 0;
        }
        buf += len;
        size -= len;
    }
    return 0;
}
