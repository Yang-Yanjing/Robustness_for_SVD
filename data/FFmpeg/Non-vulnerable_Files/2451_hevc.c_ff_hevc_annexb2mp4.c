int ff_hevc_annexb2mp4(AVIOContext *pb, const uint8_t *buf_in,
                       int size, int filter_ps, int *ps_count)
{
    int num_ps = 0, ret = 0;
    uint8_t *buf, *end, *start = NULL;
    if (!filter_ps) {
        ret = ff_avc_parse_nal_units(pb, buf_in, size);
        goto end;
    }
    ret = ff_avc_parse_nal_units_buf(buf_in, &start, &size);
    if (ret < 0)
        goto end;
    ret = 0;
    buf = start;
    end = start + size;
    while (end - buf > 4) {
        uint32_t len = FFMIN(AV_RB32(buf), end - buf - 4);
        uint8_t type = (buf[4] >> 1) & 0x3f;
        buf += 4;
        switch (type) {
        case NAL_VPS:
        case NAL_SPS:
        case NAL_PPS:
            num_ps++;
            break;
        default:
            ret += 4 + len;
            avio_wb32(pb, len);
            avio_write(pb, buf, len);
            break;
        }
        buf += len;
    }
end:
    av_free(start);
    if (ps_count)
        *ps_count = num_ps;
    return ret;
}
