static void fixstride(AVFilterLink *link, AVFrame *f)
{
    AVFrame *dst = ff_default_get_video_buffer(link, f->width, f->height);
    if(!dst)
        return;
    av_frame_copy_props(dst, f);
    av_image_copy(dst->data, dst->linesize,
                  (const uint8_t **)f->data, f->linesize,
                  dst->format, dst->width, dst->height);
    av_frame_unref(f);
    av_frame_move_ref(f, dst);
    av_frame_free(&dst);
}
