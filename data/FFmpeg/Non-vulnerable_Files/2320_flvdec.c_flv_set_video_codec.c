static int flv_set_video_codec(AVFormatContext *s, AVStream *vstream,
                               int flv_codecid, int read)
{
    AVCodecContext *vcodec = vstream->codec;
    switch (flv_codecid) {
    case FLV_CODECID_H263:
        vcodec->codec_id = AV_CODEC_ID_FLV1;
        break;
    case FLV_CODECID_REALH263:
        vcodec->codec_id = AV_CODEC_ID_H263;
        break; 
    case FLV_CODECID_SCREEN:
        vcodec->codec_id = AV_CODEC_ID_FLASHSV;
        break;
    case FLV_CODECID_SCREEN2:
        vcodec->codec_id = AV_CODEC_ID_FLASHSV2;
        break;
    case FLV_CODECID_VP6:
        vcodec->codec_id = AV_CODEC_ID_VP6F;
    case FLV_CODECID_VP6A:
        if (flv_codecid == FLV_CODECID_VP6A)
            vcodec->codec_id = AV_CODEC_ID_VP6A;
        if (read) {
            if (vcodec->extradata_size != 1) {
                ff_alloc_extradata(vcodec, 1);
            }
            if (vcodec->extradata)
                vcodec->extradata[0] = avio_r8(s->pb);
            else
                avio_skip(s->pb, 1);
        }
        return 1;     
    case FLV_CODECID_H264:
        vcodec->codec_id = AV_CODEC_ID_H264;
        vstream->need_parsing = AVSTREAM_PARSE_HEADERS;
        return 3;     
    case FLV_CODECID_MPEG4:
        vcodec->codec_id = AV_CODEC_ID_MPEG4;
        return 3;
    default:
        avpriv_request_sample(s, "Video codec (%x)", flv_codecid);
        vcodec->codec_tag = flv_codecid;
    }
    return 0;
}
