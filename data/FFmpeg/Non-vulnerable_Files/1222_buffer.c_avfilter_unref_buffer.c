void avfilter_unref_buffer(AVFilterBufferRef *ref)
{
    if (!ref)
        return;
    av_assert0(ref->buf->refcount > 0);
    if (!(--ref->buf->refcount))
        ref->buf->free(ref->buf);
    if (ref->extended_data != ref->data)
        av_freep(&ref->extended_data);
    if (ref->video)
        av_freep(&ref->video->qp_table);
    av_freep(&ref->video);
    av_freep(&ref->audio);
    av_dict_free(&ref->metadata);
    av_free(ref);
}
