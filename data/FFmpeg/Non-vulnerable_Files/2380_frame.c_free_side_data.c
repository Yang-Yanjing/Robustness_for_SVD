static void free_side_data(AVFrameSideData **ptr_sd)
{
    AVFrameSideData *sd = *ptr_sd;
    av_buffer_unref(&sd->buf);
    av_dict_free(&sd->metadata);
    av_freep(ptr_sd);
}
