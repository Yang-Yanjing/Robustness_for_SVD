static void vertical_frame_pack(FramepackContext *s,
                                AVFrame *dst,
                                int interleaved)
{
    int plane, offset;
    int length = dst->width;
    int lines  = dst->height / 2;
    for (plane = 0; plane < s->pix_desc->nb_components; plane++) {
        if (plane == 1 || plane == 2) {
            length = -(-(dst->width)      >> s->pix_desc->log2_chroma_w);
            lines  = -(-(dst->height / 2) >> s->pix_desc->log2_chroma_h);
        }
        offset = interleaved ? dst->linesize[plane] : dst->linesize[plane] * lines;
        av_image_copy_plane(dst->data[plane],
                            dst->linesize[plane] << interleaved,
                            s->input_views[LEFT]->data[plane],
                            s->input_views[LEFT]->linesize[plane],
                            length, lines);
        av_image_copy_plane(dst->data[plane] + offset,
                            dst->linesize[plane] << interleaved,
                            s->input_views[RIGHT]->data[plane],
                            s->input_views[RIGHT]->linesize[plane],
                            length, lines);
    }
}
