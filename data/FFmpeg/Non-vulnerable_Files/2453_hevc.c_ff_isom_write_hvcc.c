int ff_isom_write_hvcc(AVIOContext *pb, const uint8_t *data,
                       int size, int ps_array_completeness)
{
    int ret = 0;
    uint8_t *buf, *end, *start = NULL;
    HEVCDecoderConfigurationRecord hvcc;
    hvcc_init(&hvcc);
    if (size < 6) {
        
        ret = AVERROR_INVALIDDATA;
        goto end;
    } else if (*data == 1) {
        
        avio_write(pb, data, size);
        goto end;
    } else if (!(AV_RB24(data) == 1 || AV_RB32(data) == 1)) {
        
        ret = AVERROR_INVALIDDATA;
        goto end;
    }
    ret = ff_avc_parse_nal_units_buf(data, &start, &size);
    if (ret < 0)
        goto end;
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
        case NAL_SEI_PREFIX:
        case NAL_SEI_SUFFIX:
            ret = hvcc_add_nal_unit(buf, len, ps_array_completeness, &hvcc);
            if (ret < 0)
                goto end;
            break;
        default:
            break;
        }
        buf += len;
    }
    ret = hvcc_write(pb, &hvcc);
end:
    hvcc_close(&hvcc);
    av_free(start);
    return ret;
}
