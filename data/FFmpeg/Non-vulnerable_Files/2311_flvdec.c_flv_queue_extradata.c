static int flv_queue_extradata(FLVContext *flv, AVIOContext *pb, int stream,
                               int size)
{
    av_free(flv->new_extradata[stream]);
    flv->new_extradata[stream] = av_mallocz(size +
                                            AV_INPUT_BUFFER_PADDING_SIZE);
    if (!flv->new_extradata[stream])
        return AVERROR(ENOMEM);
    flv->new_extradata_size[stream] = size;
    avio_read(pb, flv->new_extradata[stream], size);
    return 0;
}
