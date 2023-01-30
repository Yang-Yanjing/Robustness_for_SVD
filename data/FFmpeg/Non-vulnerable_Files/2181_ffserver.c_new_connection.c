static void new_connection(int server_fd, int is_rtsp)
{
    struct sockaddr_in from_addr;
    socklen_t len;
    int fd;
    HTTPContext *c = NULL;
    len = sizeof(from_addr);
    fd = accept(server_fd, (struct sockaddr *)&from_addr,
                &len);
    if (fd < 0) {
        http_log("error during accept %s\n", strerror(errno));
        return;
    }
    if (ff_socket_nonblock(fd, 1) < 0)
        av_log(NULL, AV_LOG_WARNING, "ff_socket_nonblock failed\n");
    if (nb_connections >= config.nb_max_connections) {
        http_send_too_busy_reply(fd);
        goto fail;
    }
    
    c = av_mallocz(sizeof(HTTPContext));
    if (!c)
        goto fail;
    c->fd = fd;
    c->poll_entry = NULL;
    c->from_addr = from_addr;
    c->buffer_size = IOBUFFER_INIT_SIZE;
    c->buffer = av_malloc(c->buffer_size);
    if (!c->buffer)
        goto fail;
    c->next = first_http_ctx;
    first_http_ctx = c;
    nb_connections++;
    start_wait_request(c, is_rtsp);
    return;
 fail:
    if (c) {
        av_freep(&c->buffer);
        av_free(c);
    }
    closesocket(fd);
}
