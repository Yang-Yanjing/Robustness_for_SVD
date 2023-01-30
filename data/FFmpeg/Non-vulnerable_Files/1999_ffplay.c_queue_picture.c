static int queue_picture(VideoState *is, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)
{
    Frame *vp;
#if defined(DEBUG_SYNC)
    printf("frame_type=%c pts=%0.3f\n",
           av_get_picture_type_char(src_frame->pict_type), pts);
#endif
    if (!(vp = frame_queue_peek_writable(&is->pictq)))
        return -1;
    vp->sar = src_frame->sample_aspect_ratio;
    vp->uploaded = 0;
    vp->width = src_frame->width;
    vp->height = src_frame->height;
    vp->format = src_frame->format;
    vp->pts = pts;
    vp->duration = duration;
    vp->pos = pos;
    vp->serial = serial;
    set_default_window_size(vp->width, vp->height, vp->sar);
    av_frame_move_ref(vp->frame, src_frame);
    frame_queue_push(&is->pictq);
    return 0;
}
