static int get_video_private_data(struct Track *track, AVCodecContext *codec)
{
    AVIOContext *io = NULL;
    uint16_t sps_size, pps_size;
    int err;
    if (codec->codec_id == AV_CODEC_ID_VC1)
        return get_private_data(track, codec);
    if ((err = avio_open_dyn_buf(&io)) < 0)
        goto fail;
    err = AVERROR(EINVAL);
    if (codec->extradata_size < 11 || codec->extradata[0] != 1)
        goto fail;
    sps_size = AV_RB16(&codec->extradata[6]);
    if (11 + sps_size > codec->extradata_size)
        goto fail;
    avio_wb32(io, 0x00000001);
    avio_write(io, &codec->extradata[8], sps_size);
    pps_size = AV_RB16(&codec->extradata[9 + sps_size]);
    if (11 + sps_size + pps_size > codec->extradata_size)
        goto fail;
    avio_wb32(io, 0x00000001);
    avio_write(io, &codec->extradata[11 + sps_size], pps_size);
    err = 0;
fail:
    track->codec_private_size = avio_close_dyn_buf(io, &track->codec_private);
    return err;
}
