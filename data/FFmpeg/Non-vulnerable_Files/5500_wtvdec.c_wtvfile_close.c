static void wtvfile_close(AVIOContext *pb)
{
    WtvFile *wf = pb->opaque;
    av_freep(&wf->sectors);
    av_freep(&pb->opaque);
    av_freep(&pb->buffer);
    av_free(pb);
}
