static int flv_same_video_codec(AVCodecContext *vcodec, int flags)
{
    int flv_codecid = flags & FLV_VIDEO_CODECID_MASK;
    if (!vcodec->codec_id && !vcodec->codec_tag)
        return 1;
    switch (flv_codecid) {
    case FLV_CODECID_H263:
        return vcodec->codec_id == AV_CODEC_ID_FLV1;
    case FLV_CODECID_SCREEN:
        return vcodec->codec_id == AV_CODEC_ID_FLASHSV;
    case FLV_CODECID_SCREEN2:
        return vcodec->codec_id == AV_CODEC_ID_FLASHSV2;
    case FLV_CODECID_VP6:
        return vcodec->codec_id == AV_CODEC_ID_VP6F;
    case FLV_CODECID_VP6A:
        return vcodec->codec_id == AV_CODEC_ID_VP6A;
    case FLV_CODECID_H264:
        return vcodec->codec_id == AV_CODEC_ID_H264;
    default:
        return vcodec->codec_tag == flv_codecid;
    }
}
