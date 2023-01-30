static int ebml_read_binary(AVIOContext *pb, int length, EbmlBin *bin)
{
    av_fast_padded_malloc(&bin->data, &bin->size, length);
    if (!bin->data)
        return AVERROR(ENOMEM);
    bin->size = length;
    bin->pos  = avio_tell(pb);
    if (avio_read(pb, bin->data, length) != length) {
        av_freep(&bin->data);
        bin->size = 0;
        return AVERROR(EIO);
    }
    return 0;
}
