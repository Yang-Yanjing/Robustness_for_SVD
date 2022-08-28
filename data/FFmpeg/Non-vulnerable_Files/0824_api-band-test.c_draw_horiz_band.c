int draw_horiz_band_called;
static void draw_horiz_band(AVCodecContext *ctx, const AVFrame *fr, int offset[4],
                            int slice_position, int type, int height)
{
    int i;
    const AVPixFmtDescriptor *pix_fmt_desc;
    int chroma_w, chroma_h;
    int shift_slice_position;
    int shift_height;
    draw_horiz_band_called = 1;
    pix_fmt_desc = av_pix_fmt_desc_get(ctx->pix_fmt);
    chroma_w = -((-ctx->width) >> pix_fmt_desc->log2_chroma_w);
    chroma_h = -((-height) >> pix_fmt_desc->log2_chroma_h);
    shift_slice_position = -((-slice_position) >> pix_fmt_desc->log2_chroma_h);
    shift_height = -((-ctx->height) >> pix_fmt_desc->log2_chroma_h);
    for (i = 0; i < height; i++) {
        memcpy(slice_byte_buffer + ctx->width * slice_position + i * ctx->width,
               fr->data[0] + offset[0] + i * fr->linesize[0], ctx->width);
    }
    for (i = 0; i < chroma_h; i++) {
        memcpy(slice_byte_buffer + ctx->width * ctx->height + chroma_w * shift_slice_position + i * chroma_w,
               fr->data[1] + offset[1] + i * fr->linesize[1], chroma_w);
    }
    for (i = 0; i < chroma_h; i++) {
        memcpy(slice_byte_buffer + ctx->width * ctx->height + chroma_w * shift_height + chroma_w * shift_slice_position + i * chroma_w,
               fr->data[2] + offset[2] + i * fr->linesize[2], chroma_w);
    }
}
