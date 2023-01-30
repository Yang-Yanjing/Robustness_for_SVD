static void find_index_range(AVFormatContext *s, const char *full_path,
                             int64_t pos, int *index_length)
{
    uint8_t buf[8];
    URLContext *fd;
    int ret;
    ret = ffurl_open(&fd, full_path, AVIO_FLAG_READ, &s->interrupt_callback, NULL);
    if (ret < 0)
        return;
    if (ffurl_seek(fd, pos, SEEK_SET) != pos) {
        ffurl_close(fd);
        return;
    }
    ret = ffurl_read(fd, buf, 8);
    ffurl_close(fd);
    if (ret < 8)
        return;
    if (AV_RL32(&buf[4]) != MKTAG('s', 'i', 'd', 'x'))
        return;
    *index_length = AV_RB32(&buf[0]);
}
