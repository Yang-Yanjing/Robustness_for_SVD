static void horizontal_frame_pack(FramepackContext *s,
                                  AVFrame *dst,
                                  int interleaved)
{
    int plane, i;
    int length = dst->width / 2;
    int lines  = dst->height;
    for (plane = 0; plane < s->pix_desc->nb_components; plane++) {
        const uint8_t *leftp  = s->input_views[LEFT]->data[plane];
        const uint8_t *rightp = s->input_views[RIGHT]->data[plane];
        uint8_t *dstp         = dst->data[plane];
        if (plane == 1 || plane == 2) {
            length = FF_CEIL_RSHIFT(dst->width / 2, s->pix_desc->log2_chroma_w);
            lines  = FF_CEIL_RSHIFT(dst->height,    s->pix_desc->log2_chroma_h);
        }
        if (interleaved) {
            for (i = 0; i < lines; i++) {
                int j;
                int k = 0;
                for (j = 0; j < length; j++) {
                    dstp[k++] = leftp[j];
                    dstp[k++] = rightp[j];
                }
                dstp   += dst->linesize[plane];
                leftp  += s->input_views[LEFT]->linesize[plane];
                rightp += s->input_views[RIGHT]->linesize[plane];
            }
        } else {
            av_image_copy_plane(dst->data[plane], dst->linesize[plane],
                                leftp, s->input_views[LEFT]->linesize[plane],
                                length, lines);
            av_image_copy_plane(dst->data[plane] + length, dst->linesize[plane],
                                rightp, s->input_views[RIGHT]->linesize[plane],
                                length, lines);
        }
    }
}
