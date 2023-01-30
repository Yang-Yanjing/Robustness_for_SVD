static int dc1394_v1_close(AVFormatContext * context)
{
    struct dc1394_data *dc1394 = context->priv_data;
    dc1394_stop_iso_transmission(dc1394->handle, dc1394->camera.node);
    dc1394_dma_unlisten(dc1394->handle, &dc1394->camera);
    dc1394_dma_release_camera(dc1394->handle, &dc1394->camera);
    dc1394_destroy_handle(dc1394->handle);
    return 0;
}
