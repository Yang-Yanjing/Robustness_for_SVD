int main(int argc, char **argv)
{
    AVBPrint src;
    char c, *src_buf, *recv_buf;
    int recv_buf_size, ret;
    void *zmq_ctx, *socket;
    const char *bind_address = "tcp://localhost:5555";
    const char *infilename = NULL;
    FILE *infile = NULL;
    zmq_msg_t msg;
    while ((c = getopt(argc, argv, "b:hi:")) != -1) {
        switch (c) {
        case 'b':
            bind_address = optarg;
            break;
        case 'h':
            usage();
            return 0;
        case 'i':
            infilename = optarg;
            break;
        case '?':
            return 1;
        }
    }
    if (!infilename || !strcmp(infilename, "-")) {
        infilename = "stdin";
        infile = stdin;
    } else {
        infile = fopen(infilename, "r");
    }
    if (!infile) {
        av_log(NULL, AV_LOG_ERROR,
               "Impossible to open input file '%s': %s\n", infilename, strerror(errno));
        return 1;
    }
    zmq_ctx = zmq_ctx_new();
    if (!zmq_ctx) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not create ZMQ context: %s\n", zmq_strerror(errno));
        return 1;
    }
    socket = zmq_socket(zmq_ctx, ZMQ_REQ);
    if (!socket) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not create ZMQ socket: %s\n", zmq_strerror(errno));
        ret = 1;
        goto end;
    }
    if (zmq_connect(socket, bind_address) == -1) {
        av_log(NULL, AV_LOG_ERROR, "Could not bind ZMQ responder to address '%s': %s\n",
               bind_address, zmq_strerror(errno));
        ret = 1;
        goto end;
    }
    
    av_bprint_init(&src, 1, AV_BPRINT_SIZE_UNLIMITED);
    while ((c = fgetc(infile)) != EOF)
        av_bprint_chars(&src, c, 1);
    av_bprint_chars(&src, 0, 1);
    if (!av_bprint_is_complete(&src)) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate a buffer for the source string\n");
        av_bprint_finalize(&src, NULL);
        ret = 1;
        goto end;
    }
    av_bprint_finalize(&src, &src_buf);
    if (zmq_send(socket, src_buf, strlen(src_buf), 0) == -1) {
        av_log(NULL, AV_LOG_ERROR, "Could not send message: %s\n", zmq_strerror(errno));
        ret = 1;
        goto end;
    }
    if (zmq_msg_init(&msg) == -1) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not initialize receiving message: %s\n", zmq_strerror(errno));
        ret = 1;
        goto end;
    }
    if (zmq_msg_recv(&msg, socket, 0) == -1) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not receive message: %s\n", zmq_strerror(errno));
        zmq_msg_close(&msg);
        ret = 1;
        goto end;
    }
    recv_buf_size = zmq_msg_size(&msg) + 1;
    recv_buf = av_malloc(recv_buf_size);
    if (!recv_buf) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not allocate receiving message buffer\n");
        zmq_msg_close(&msg);
        ret = 1;
        goto end;
    }
    memcpy(recv_buf, zmq_msg_data(&msg), recv_buf_size);
    recv_buf[recv_buf_size-1] = 0;
    printf("%s\n", recv_buf);
    zmq_msg_close(&msg);
    av_free(recv_buf);
end:
    zmq_close(socket);
    zmq_ctx_destroy(zmq_ctx);
    return ret;
}
