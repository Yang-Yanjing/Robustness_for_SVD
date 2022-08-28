static void sub2video_push_ref(InputStream *ist, int64_t pts)
{
    AVFrame *frame = ist->sub2video.frame;
    int i;
    av_assert1(frame->data[0]);
    ist->sub2video.last_pts = frame->pts = pts;
    for (i = 0; i < ist->nb_filters; i++)
        av_buffersrc_add_frame_flags(ist->filters[i]->filter, frame,
                                     AV_BUFFERSRC_FLAG_KEEP_REF |
                                     AV_BUFFERSRC_FLAG_PUSH);
}
