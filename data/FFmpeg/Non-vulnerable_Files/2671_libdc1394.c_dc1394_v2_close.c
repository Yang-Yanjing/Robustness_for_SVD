static int dc1394_v2_close(AVFormatContext * context)
{
    struct dc1394_data *dc1394 = context->priv_data;
    dc1394_video_set_transmission(dc1394->camera, DC1394_OFF);
    dc1394_capture_stop(dc1394->camera);
    dc1394_camera_free(dc1394->camera);
    dc1394_free(dc1394->d);
    return 0;
}
