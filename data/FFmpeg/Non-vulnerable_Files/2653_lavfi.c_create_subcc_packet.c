static int create_subcc_packet(AVFormatContext *avctx, AVFrame *frame,
                               int sink_idx)
{
    LavfiContext *lavfi = avctx->priv_data;
    AVFrameSideData *sd;
    int stream_idx, i, ret;
    if ((stream_idx = lavfi->sink_stream_subcc_map[sink_idx]) < 0)
        return 0;
    for (i = 0; i < frame->nb_side_data; i++)
        if (frame->side_data[i]->type == AV_FRAME_DATA_A53_CC)
            break;
    if (i >= frame->nb_side_data)
        return 0;
    sd = frame->side_data[i];
    if ((ret = av_new_packet(&lavfi->subcc_packet, sd->size)) < 0)
        return ret;
    memcpy(lavfi->subcc_packet.data, sd->data, sd->size);
    lavfi->subcc_packet.stream_index = stream_idx;
    lavfi->subcc_packet.pts = frame->pts;
    lavfi->subcc_packet.pos = av_frame_get_pkt_pos(frame);
    return 0;
}
