static int flv_read_close(AVFormatContext *s)
{
    int i;
    FLVContext *flv = s->priv_data;
    for (i=0; i<FLV_STREAM_TYPE_NB; i++)
        av_freep(&flv->new_extradata[i]);
    return 0;
}
