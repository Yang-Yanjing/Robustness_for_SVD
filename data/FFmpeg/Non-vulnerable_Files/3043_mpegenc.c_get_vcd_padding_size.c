static int get_vcd_padding_size(AVFormatContext *ctx, int64_t pts)
{
    MpegMuxContext *s = ctx->priv_data;
    int pad_bytes = 0;
    if (s->vcd_padding_bitrate_num > 0 && pts != AV_NOPTS_VALUE) {
        int64_t full_pad_bytes;
        
        full_pad_bytes =
            av_rescale(s->vcd_padding_bitrate_num, pts, 90000LL * 8 * VCD_PADDING_BITRATE_DEN);
        pad_bytes = (int)(full_pad_bytes - s->vcd_padding_bytes_written);
        if (pad_bytes < 0)
            

            pad_bytes = 0;
    }
    return pad_bytes;
}
