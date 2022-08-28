static int get_video_buffer(AVFrame *frame, int align)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(frame->format);
    int ret, i;
    if (!desc)
        return AVERROR(EINVAL);
    if ((ret = av_image_check_size(frame->width, frame->height, 0, NULL)) < 0)
        return ret;
    if (!frame->linesize[0]) {
        for(i=1; i<=align; i+=i) {
            ret = av_image_fill_linesizes(frame->linesize, frame->format,
                                          FFALIGN(frame->width, i));
            if (ret < 0)
                return ret;
            if (!(frame->linesize[0] & (align-1)))
                break;
        }
        for (i = 0; i < 4 && frame->linesize[i]; i++)
            frame->linesize[i] = FFALIGN(frame->linesize[i], align);
    }
    for (i = 0; i < 4 && frame->linesize[i]; i++) {
        int h = FFALIGN(frame->height, 32);
        if (i == 1 || i == 2)
            h = FF_CEIL_RSHIFT(h, desc->log2_chroma_h);
        frame->buf[i] = av_buffer_alloc(frame->linesize[i] * h + 16 + 16 - 1);
        if (!frame->buf[i])
            goto fail;
        frame->data[i] = frame->buf[i]->data;
    }
    if (desc->flags & AV_PIX_FMT_FLAG_PAL || desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL) {
        av_buffer_unref(&frame->buf[1]);
        frame->buf[1] = av_buffer_alloc(1024);
        if (!frame->buf[1])
            goto fail;
        frame->data[1] = frame->buf[1]->data;
    }
    frame->extended_data = frame->data;
    return 0;
fail:
    av_frame_unref(frame);
    return AVERROR(ENOMEM);
}
