int av_image_copy_to_buffer(uint8_t *dst, int dst_size,
                            const uint8_t * const src_data[4],
                            const int src_linesize[4],
                            enum AVPixelFormat pix_fmt,
                            int width, int height, int align)
{
    int i, j, nb_planes = 0, linesize[4];
    int size = av_image_get_buffer_size(pix_fmt, width, height, align);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    if (size > dst_size || size < 0 || !desc)
        return AVERROR(EINVAL);
    for (i = 0; i < desc->nb_components; i++)
        nb_planes = FFMAX(desc->comp[i].plane, nb_planes);
    nb_planes++;
    av_image_fill_linesizes(linesize, pix_fmt, width);
    for (i = 0; i < nb_planes; i++) {
        int h, shift = (i == 1 || i == 2) ? desc->log2_chroma_h : 0;
        const uint8_t *src = src_data[i];
        h = (height + (1 << shift) - 1) >> shift;
        for (j = 0; j < h; j++) {
            memcpy(dst, src, linesize[i]);
            dst += FFALIGN(linesize[i], align);
            src += src_linesize[i];
        }
    }
    if (desc->flags & AV_PIX_FMT_FLAG_PAL) {
        uint32_t *d32 = (uint32_t *)(((size_t)dst + 3) & ~3);
        for (i = 0; i<256; i++)
            AV_WL32(d32 + i, AV_RN32(src_data[1] + 4*i));
    }
    return size;
}
