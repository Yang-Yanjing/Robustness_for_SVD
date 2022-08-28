static int dc1394_v1_read_header(AVFormatContext *c)
{
    dc1394_data* dc1394 = c->priv_data;
    AVStream* vst;
    nodeid_t* camera_nodes;
    int res;
    struct dc1394_frame_format *fmt = NULL;
    struct dc1394_frame_rate *fps = NULL;
    if (dc1394_read_common(c, &fmt, &fps) != 0)
        return -1;
    
    dc1394->handle = dc1394_create_handle(0); 
    if (!dc1394->handle) {
        av_log(c, AV_LOG_ERROR, "Can't acquire dc1394 handle on port %d\n", 0 );
        goto out;
    }
    camera_nodes = dc1394_get_camera_nodes(dc1394->handle, &res, 1);
    if (!camera_nodes || camera_nodes[dc1394->channel] == DC1394_NO_CAMERA) {
        av_log(c, AV_LOG_ERROR, "There's no IIDC camera on the channel %d\n", dc1394->channel);
        goto out_handle;
    }
    res = dc1394_dma_setup_capture(dc1394->handle, camera_nodes[dc1394->channel],
                                   0,
                                   FORMAT_VGA_NONCOMPRESSED,
                                   fmt->frame_size_id,
                                   SPEED_400,
                                   fps->frame_rate_id, 8, 1,
                                   c->filename,
                                   &dc1394->camera);
    dc1394_free_camera_nodes(camera_nodes);
    if (res != DC1394_SUCCESS) {
        av_log(c, AV_LOG_ERROR, "Can't prepare camera for the DMA capture\n");
        goto out_handle;
    }
    res = dc1394_start_iso_transmission(dc1394->handle, dc1394->camera.node);
    if (res != DC1394_SUCCESS) {
        av_log(c, AV_LOG_ERROR, "Can't start isochronous transmission\n");
        goto out_handle_dma;
    }
    return 0;
out_handle_dma:
    dc1394_dma_unlisten(dc1394->handle, &dc1394->camera);
    dc1394_dma_release_camera(dc1394->handle, &dc1394->camera);
out_handle:
    dc1394_destroy_handle(dc1394->handle);
out:
    return -1;
}
