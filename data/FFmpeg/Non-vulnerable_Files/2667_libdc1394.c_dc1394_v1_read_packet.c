static int dc1394_v1_read_packet(AVFormatContext *c, AVPacket *pkt)
{
    struct dc1394_data *dc1394 = c->priv_data;
    int res;
    
    if (dc1394->current_frame++) {
        if (dc1394_dma_done_with_buffer(&dc1394->camera) != DC1394_SUCCESS)
            av_log(c, AV_LOG_ERROR, "failed to release %d frame\n", dc1394->current_frame);
    }
    res = dc1394_dma_single_capture(&dc1394->camera);
    if (res == DC1394_SUCCESS) {
        dc1394->packet.data = (uint8_t *)(dc1394->camera.capture_buffer);
        dc1394->packet.pts = (dc1394->current_frame * 1000000) / dc1394->frame_rate;
        res = dc1394->packet.size;
    } else {
        av_log(c, AV_LOG_ERROR, "DMA capture failed\n");
        dc1394->packet.data = NULL;
        res = -1;
    }
    *pkt = dc1394->packet;
    return res;
}
