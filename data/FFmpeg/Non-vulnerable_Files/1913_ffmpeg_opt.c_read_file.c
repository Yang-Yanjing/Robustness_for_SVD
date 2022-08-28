static uint8_t *read_file(const char *filename)
{
    AVIOContext *pb      = NULL;
    AVIOContext *dyn_buf = NULL;
    int ret = avio_open(&pb, filename, AVIO_FLAG_READ);
    uint8_t buf[1024], *str;
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error opening file %s.\n", filename);
        return NULL;
    }
    ret = avio_open_dyn_buf(&dyn_buf);
    if (ret < 0) {
        avio_closep(&pb);
        return NULL;
    }
    while ((ret = avio_read(pb, buf, sizeof(buf))) > 0)
        avio_write(dyn_buf, buf, ret);
    avio_w8(dyn_buf, 0);
    avio_closep(&pb);
    ret = avio_close_dyn_buf(dyn_buf, &str);
    if (ret < 0)
        return NULL;
    return str;
}
