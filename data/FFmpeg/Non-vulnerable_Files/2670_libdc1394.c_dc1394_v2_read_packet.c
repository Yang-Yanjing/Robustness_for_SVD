static int dc1394_v2_read_packet(AVFormatContext *c, AVPacket *pkt)
{
    struct dc1394_data *dc1394 = c->priv_data;
    int res;
    
    if (dc1394->current_frame++) {
        if (dc1394_capture_enqueue(dc1394->camera, dc1394->frame) != DC1394_SUCCESS)
            av_log(c, AV_LOG_ERROR, "failed to release %d frame\n", dc1394->current_frame);
    }
    res = dc1394_capture_dequeue(dc1394->camera, DC1394_CAPTURE_POLICY_WAIT, &dc1394->frame);
    if (res == DC1394_SUCCESS) {
        dc1394->packet.data = (uint8_t *) dc1394->frame->image;
        dc1394->packet.pts  = dc1394->current_frame * 1000000 / dc1394->frame_rate;
        res = dc1394->frame->image_bytes;
    } else {
        av_log(c, AV_LOG_ERROR, "DMA capture failed\n");
        dc1394->packet.data = NULL;
        res = -1;
    }
    *pkt = dc1394->packet;
    return res;
}
