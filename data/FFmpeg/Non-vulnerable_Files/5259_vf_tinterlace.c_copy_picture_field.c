static inline
void copy_picture_field(TInterlaceContext *tinterlace,
                        uint8_t *dst[4], int dst_linesize[4],
                        const uint8_t *src[4], int src_linesize[4],
                        enum AVPixelFormat format, int w, int src_h,
                        int src_field, int interleave, int dst_field,
                        int flags)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(format);
    int hsub = desc->log2_chroma_w;
    int plane, vsub = desc->log2_chroma_h;
    int k = src_field == FIELD_UPPER_AND_LOWER ? 1 : 2;
    int h;
    for (plane = 0; plane < desc->nb_components; plane++) {
        int lines = plane == 1 || plane == 2 ? FF_CEIL_RSHIFT(src_h, vsub) : src_h;
        int cols  = plane == 1 || plane == 2 ? FF_CEIL_RSHIFT(    w, hsub) : w;
        uint8_t *dstp = dst[plane];
        const uint8_t *srcp = src[plane];
        lines = (lines + (src_field == FIELD_UPPER)) / k;
        if (src_field == FIELD_LOWER)
            srcp += src_linesize[plane];
        if (interleave && dst_field == FIELD_LOWER)
            dstp += dst_linesize[plane];
        if (flags & TINTERLACE_FLAG_VLPF) {
            
            
            
            int srcp_linesize = src_linesize[plane] * k;
            int dstp_linesize = dst_linesize[plane] * (interleave ? 2 : 1);
            for (h = lines; h > 0; h--) {
                const uint8_t *srcp_above = srcp - src_linesize[plane];
                const uint8_t *srcp_below = srcp + src_linesize[plane];
                if (h == lines) srcp_above = srcp; 
                if (h == 1) srcp_below = srcp;     
                tinterlace->lowpass_line(dstp, cols, srcp, srcp_above, srcp_below);
                dstp += dstp_linesize;
                srcp += srcp_linesize;
            }
        } else {
            av_image_copy_plane(dstp, dst_linesize[plane] * (interleave ? 2 : 1),
                                srcp, src_linesize[plane]*k, cols, lines);
        }
    }
}
