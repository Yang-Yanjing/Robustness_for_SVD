static int rm_read_extradata(AVIOContext *pb, AVCodecContext *avctx, unsigned size)
{
    if (size >= 1<<24) {
        av_log(avctx, AV_LOG_ERROR, "extradata size %u too large\n", size);
        return -1;
    }
    if (ff_get_extradata(avctx, pb, size) < 0)
        return AVERROR(ENOMEM);
    return 0;
}
