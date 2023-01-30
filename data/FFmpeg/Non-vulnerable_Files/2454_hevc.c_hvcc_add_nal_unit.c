static int hvcc_add_nal_unit(uint8_t *nal_buf, uint32_t nal_size,
                             int ps_array_completeness,
                             HEVCDecoderConfigurationRecord *hvcc)
{
    int ret = 0;
    GetBitContext gbc;
    uint8_t nal_type;
    uint8_t *rbsp_buf;
    uint32_t rbsp_size;
    rbsp_buf = nal_unit_extract_rbsp(nal_buf, nal_size, &rbsp_size);
    if (!rbsp_buf) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    ret = init_get_bits8(&gbc, rbsp_buf, rbsp_size);
    if (ret < 0)
        goto end;
    nal_unit_parse_header(&gbc, &nal_type);
    




    switch (nal_type) {
    case NAL_VPS:
    case NAL_SPS:
    case NAL_PPS:
    case NAL_SEI_PREFIX:
    case NAL_SEI_SUFFIX:
        ret = hvcc_array_add_nal_unit(nal_buf, nal_size, nal_type,
                                      ps_array_completeness, hvcc);
        if (ret < 0)
            goto end;
        else if (nal_type == NAL_VPS)
            ret = hvcc_parse_vps(&gbc, hvcc);
        else if (nal_type == NAL_SPS)
            ret = hvcc_parse_sps(&gbc, hvcc);
        else if (nal_type == NAL_PPS)
            ret = hvcc_parse_pps(&gbc, hvcc);
        if (ret < 0)
            goto end;
        break;
    default:
        ret = AVERROR_INVALIDDATA;
        goto end;
    }
end:
    av_free(rbsp_buf);
    return ret;
}
