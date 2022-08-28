int ff_hevc_annexb2mp4_buf(const uint8_t *buf_in, uint8_t **buf_out,
                           int *size, int filter_ps, int *ps_count)
{
    AVIOContext *pb;
    int ret;
    ret = avio_open_dyn_buf(&pb);
    if (ret < 0)
        return ret;
    ret   = ff_hevc_annexb2mp4(pb, buf_in, *size, filter_ps, ps_count);
    *size = avio_close_dyn_buf(pb, buf_out);
    return ret;
}
