static int mov_write_isml_manifest(AVIOContext *pb, MOVMuxContext *mov)
{
    int64_t pos = avio_tell(pb);
    int i;
    static const uint8_t uuid[] = {
        0xa5, 0xd4, 0x0b, 0x30, 0xe8, 0x14, 0x11, 0xdd,
        0xba, 0x2f, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
    };
    avio_wb32(pb, 0);
    ffio_wfourcc(pb, "uuid");
    avio_write(pb, uuid, sizeof(uuid));
    avio_wb32(pb, 0);
    avio_printf(pb, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    avio_printf(pb, "<smil xmlns=\"http://www.w3.org/2001/SMIL20/Language\">\n");
    avio_printf(pb, "<head>\n");
    if (!(mov->fc->flags & AVFMT_FLAG_BITEXACT))
        avio_printf(pb, "<meta name=\"creator\" content=\"%s\" />\n",
                    LIBAVFORMAT_IDENT);
    avio_printf(pb, "</head>\n");
    avio_printf(pb, "<body>\n");
    avio_printf(pb, "<switch>\n");
    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        const char *type;
        

        int track_id = i + 1;
        if (track->enc->codec_type == AVMEDIA_TYPE_VIDEO) {
            type = "video";
        } else if (track->enc->codec_type == AVMEDIA_TYPE_AUDIO) {
            type = "audio";
        } else {
            continue;
        }
        avio_printf(pb, "<%s systemBitrate=\"%d\">\n", type,
                                                       track->enc->bit_rate);
        param_write_int(pb, "systemBitrate", track->enc->bit_rate);
        param_write_int(pb, "trackID", track_id);
        if (track->enc->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (track->enc->codec_id == AV_CODEC_ID_H264) {
                uint8_t *ptr;
                int size = track->enc->extradata_size;
                if (!ff_avc_write_annexb_extradata(track->enc->extradata, &ptr,
                                                   &size)) {
                    param_write_hex(pb, "CodecPrivateData",
                                    ptr ? ptr : track->enc->extradata,
                                    size);
                    av_free(ptr);
                }
                param_write_string(pb, "FourCC", "H264");
            } else if (track->enc->codec_id == AV_CODEC_ID_VC1) {
                param_write_string(pb, "FourCC", "WVC1");
                param_write_hex(pb, "CodecPrivateData", track->enc->extradata,
                                track->enc->extradata_size);
            }
            param_write_int(pb, "MaxWidth", track->enc->width);
            param_write_int(pb, "MaxHeight", track->enc->height);
            param_write_int(pb, "DisplayWidth", track->enc->width);
            param_write_int(pb, "DisplayHeight", track->enc->height);
        } else {
            if (track->enc->codec_id == AV_CODEC_ID_AAC) {
                param_write_string(pb, "FourCC", "AACL");
            } else if (track->enc->codec_id == AV_CODEC_ID_WMAPRO) {
                param_write_string(pb, "FourCC", "WMAP");
            }
            param_write_hex(pb, "CodecPrivateData", track->enc->extradata,
                            track->enc->extradata_size);
            param_write_int(pb, "AudioTag", ff_codec_get_tag(ff_codec_wav_tags,
                                                             track->enc->codec_id));
            param_write_int(pb, "Channels", track->enc->channels);
            param_write_int(pb, "SamplingRate", track->enc->sample_rate);
            param_write_int(pb, "BitsPerSample", 16);
            param_write_int(pb, "PacketSize", track->enc->block_align ?
                                              track->enc->block_align : 4);
        }
        avio_printf(pb, "</%s>\n", type);
    }
    avio_printf(pb, "</switch>\n");
    avio_printf(pb, "</body>\n");
    avio_printf(pb, "</smil>\n");
    return update_size(pb, pos);
}
