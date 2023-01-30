AVFilterBufferRef *
avfilter_get_video_buffer_ref_from_arrays(uint8_t * const data[4], const int linesize[4], int perms,
                                          int w, int h, enum AVPixelFormat format)
{
    AVFilterBuffer *pic = av_mallocz(sizeof(AVFilterBuffer));
    AVFilterBufferRef *picref = av_mallocz(sizeof(AVFilterBufferRef));
    if (!pic || !picref)
        goto fail;
    picref->buf = pic;
    picref->buf->free = ff_avfilter_default_free_buffer;
    if (!(picref->video = av_mallocz(sizeof(AVFilterBufferRefVideoProps))))
        goto fail;
    pic->w = picref->video->w = w;
    pic->h = picref->video->h = h;
    
    picref->perms = perms | AV_PERM_READ;
    pic->refcount = 1;
    picref->type = AVMEDIA_TYPE_VIDEO;
    pic->format = picref->format = format;
    memcpy(pic->data,        data,          4*sizeof(data[0]));
    memcpy(pic->linesize,    linesize,      4*sizeof(linesize[0]));
    memcpy(picref->data,     pic->data,     sizeof(picref->data));
    memcpy(picref->linesize, pic->linesize, sizeof(picref->linesize));
    pic->   extended_data = pic->data;
    picref->extended_data = picref->data;
    picref->pts = AV_NOPTS_VALUE;
    return picref;
fail:
    if (picref && picref->video)
        av_free(picref->video);
    av_free(picref);
    av_free(pic);
    return NULL;
}
