static int write_sm_data(AVFormatContext *s, AVIOContext *bc, AVPacket *pkt, int is_meta)
{
    int ret, i, dyn_size;
    unsigned flags;
    AVIOContext *dyn_bc;
    int sm_data_count = 0;
    uint8_t tmp[256];
    uint8_t *dyn_buf;
    ret = avio_open_dyn_buf(&dyn_bc);
    if (ret < 0)
        return ret;
    for (i = 0; i<pkt->side_data_elems; i++) {
        const uint8_t *data = pkt->side_data[i].data;
        int size = pkt->side_data[i].size;
        const uint8_t *data_end = data + size;
        if (is_meta) {
            if (   pkt->side_data[i].type == AV_PKT_DATA_METADATA_UPDATE
                || pkt->side_data[i].type == AV_PKT_DATA_STRINGS_METADATA) {
                if (!size || data[size-1]) {
                    ret = AVERROR(EINVAL);
                    goto fail;
                }
                while (data < data_end) {
                    const uint8_t *key = data;
                    const uint8_t *val = data + strlen(key) + 1;
                    if(val >= data_end) {
                        ret = AVERROR(EINVAL);
                        goto fail;
                    }
                    put_str(dyn_bc, key);
                    put_s(dyn_bc, -1);
                    put_str(dyn_bc, val);
                    data = val + strlen(val) + 1;
                    sm_data_count++;
                }
            }
        } else {
            switch (pkt->side_data[i].type) {
            case AV_PKT_DATA_PALETTE:
            case AV_PKT_DATA_NEW_EXTRADATA:
            case AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL:
            default:
                if (pkt->side_data[i].type == AV_PKT_DATA_PALETTE) {
                    put_str(dyn_bc, "Palette");
                } else if(pkt->side_data[i].type == AV_PKT_DATA_NEW_EXTRADATA) {
                    put_str(dyn_bc, "Extradata");
                } else if(pkt->side_data[i].type == AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL) {
                    snprintf(tmp, sizeof(tmp), "CodecSpecificSide%"PRId64"", AV_RB64(data));
                    put_str(dyn_bc, tmp);
                } else {
                    snprintf(tmp, sizeof(tmp), "UserData%s-SD-%d",
                            (s->flags & AVFMT_FLAG_BITEXACT) ? "Lavf" : LIBAVFORMAT_IDENT,
                            pkt->side_data[i].type);
                    put_str(dyn_bc, tmp);
                }
                put_s(dyn_bc, -2);
                put_str(dyn_bc, "bin");
                ff_put_v(dyn_bc, pkt->side_data[i].size);
                avio_write(dyn_bc, data, pkt->side_data[i].size);
                sm_data_count++;
                break;
            case AV_PKT_DATA_PARAM_CHANGE:
                flags = bytestream_get_le32(&data);
                if (flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT) {
                    put_str(dyn_bc, "Channels");
                    put_s(dyn_bc, bytestream_get_le32(&data));
                    sm_data_count++;
                }
                if (flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT) {
                    put_str(dyn_bc, "ChannelLayout");
                    put_s(dyn_bc, -2);
                    put_str(dyn_bc, "u64");
                    ff_put_v(bc, 8);
                    avio_write(dyn_bc, data, 8); data+=8;
                    sm_data_count++;
                }
                if (flags & AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE) {
                    put_str(dyn_bc, "SampleRate");
                    put_s(dyn_bc, bytestream_get_le32(&data));
                    sm_data_count++;
                }
                if (flags & AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS) {
                    put_str(dyn_bc, "Width");
                    put_s(dyn_bc, bytestream_get_le32(&data));
                    put_str(dyn_bc, "Height");
                    put_s(dyn_bc, bytestream_get_le32(&data));
                    sm_data_count+=2;
                }
                break;
            case AV_PKT_DATA_SKIP_SAMPLES:
                if (AV_RL32(data)) {
                    put_str(dyn_bc, "SkipStart");
                    put_s(dyn_bc, (unsigned)AV_RL32(data));
                    sm_data_count++;
                }
                if (AV_RL32(data+4)) {
                    put_str(dyn_bc, "SkipEnd");
                    put_s(dyn_bc, (unsigned)AV_RL32(data+4));
                    sm_data_count++;
                }
                break;
            case AV_PKT_DATA_METADATA_UPDATE:
            case AV_PKT_DATA_STRINGS_METADATA:
            case AV_PKT_DATA_QUALITY_STATS:
                
                break;
            }
        }
    }
fail:
    ff_put_v(bc, sm_data_count);
    dyn_size = avio_close_dyn_buf(dyn_bc, &dyn_buf);
    avio_write(bc, dyn_buf, dyn_size);
    av_freep(&dyn_buf);
    return ret;
}
