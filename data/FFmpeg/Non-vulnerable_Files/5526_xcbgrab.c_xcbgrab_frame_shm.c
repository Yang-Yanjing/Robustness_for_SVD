static int xcbgrab_frame_shm(AVFormatContext *s, AVPacket *pkt)
{
    XCBGrabContext *c = s->priv_data;
    xcb_shm_get_image_cookie_t iq;
    xcb_shm_get_image_reply_t *img;
    xcb_drawable_t drawable = c->screen->root;
    uint8_t *data;
    int size = c->frame_size + AV_INPUT_BUFFER_PADDING_SIZE;
    int id   = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);
    xcb_generic_error_t *e = NULL;
    if (id == -1) {
        char errbuf[1024];
        int err = AVERROR(errno);
        av_strerror(err, errbuf, sizeof(errbuf));
        av_log(s, AV_LOG_ERROR, "Cannot get %d bytes of shared memory: %s.\n",
               size, errbuf);
        return err;
    }
    xcb_shm_attach(c->conn, c->segment, id, 0);
    iq = xcb_shm_get_image(c->conn, drawable,
                           c->x, c->y, c->width, c->height, ~0,
                           XCB_IMAGE_FORMAT_Z_PIXMAP, c->segment, 0);
    xcb_shm_detach(c->conn, c->segment);
    img = xcb_shm_get_image_reply(c->conn, iq, &e);
    xcb_flush(c->conn);
    if (e) {
        av_log(s, AV_LOG_ERROR,
               "Cannot get the image data "
               "event_error: response_type:%u error_code:%u "
               "sequence:%u resource_id:%u minor_code:%u major_code:%u.\n",
               e->response_type, e->error_code,
               e->sequence, e->resource_id, e->minor_code, e->major_code);
        shmctl(id, IPC_RMID, 0);
        return AVERROR(EACCES);
    }
    free(img);
    data = shmat(id, NULL, 0);
    shmctl(id, IPC_RMID, 0);
    if ((intptr_t)data == -1)
        return AVERROR(errno);
    pkt->buf = av_buffer_create(data, size, dealloc_shm, NULL, 0);
    if (!pkt->buf) {
        shmdt(data);
        return AVERROR(ENOMEM);
    }
    pkt->data = pkt->buf->data;
    pkt->size = c->frame_size;
    return 0;
}
