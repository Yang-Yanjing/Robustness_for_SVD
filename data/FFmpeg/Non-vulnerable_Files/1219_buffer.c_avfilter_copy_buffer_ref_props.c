void avfilter_copy_buffer_ref_props(AVFilterBufferRef *dst, const AVFilterBufferRef *src)
{
    
    dst->pts             = src->pts;
    dst->pos             = src->pos;
    switch (src->type) {
    case AVMEDIA_TYPE_VIDEO: {
        if (dst->video->qp_table)
            av_freep(&dst->video->qp_table);
        copy_video_props(dst->video, src->video);
        break;
    }
    case AVMEDIA_TYPE_AUDIO: *dst->audio = *src->audio; break;
    default: break;
    }
    av_dict_free(&dst->metadata);
    av_dict_copy(&dst->metadata, src->metadata, 0);
}
