int main(int argc, char **argv)
{
    AVFormatContext *input_ctx = NULL;
    AVStream *video_st = NULL;
    AVCodecContext *decoder_ctx = NULL;
    const AVCodec *decoder;
    AVPacket pkt = { 0 };
    AVFrame *frame = NULL;
    DecodeContext decode = { NULL };
    Display *dpy = NULL;
    int va_ver_major, va_ver_minor;
    mfxIMPL mfx_impl = MFX_IMPL_AUTO_ANY;
    mfxVersion mfx_ver = { { 1, 1 } };
    mfxFrameAllocator frame_allocator = {
        .pthis = &decode,
        .Alloc = frame_alloc,
        .Lock  = frame_lock,
        .Unlock = frame_unlock,
        .GetHDL = frame_get_hdl,
        .Free   = frame_free,
    };
    AVIOContext *output_ctx = NULL;
    int ret, i, err;
    av_register_all();
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }
    
    ret = avformat_open_input(&input_ctx, argv[1], NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "Cannot open input file '%s': ", argv[1]);
        goto finish;
    }
    
    for (i = 0; i < input_ctx->nb_streams; i++) {
        AVStream *st = input_ctx->streams[i];
        if (st->codec->codec_id == AV_CODEC_ID_H264 && !video_st)
            video_st = st;
        else
            st->discard = AVDISCARD_ALL;
    }
    if (!video_st) {
        fprintf(stderr, "No H.264 video stream in the input file\n");
        goto finish;
    }
    
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open the X display\n");
        goto finish;
    }
    decode.va_dpy = vaGetDisplay(dpy);
    if (!decode.va_dpy) {
        fprintf(stderr, "Cannot open the VA display\n");
        goto finish;
    }
    err = vaInitialize(decode.va_dpy, &va_ver_major, &va_ver_minor);
    if (err != VA_STATUS_SUCCESS) {
        fprintf(stderr, "Cannot initialize VA: %s\n", vaErrorStr(err));
        goto finish;
    }
    fprintf(stderr, "Initialized VA v%d.%d\n", va_ver_major, va_ver_minor);
    
    err = MFXInit(mfx_impl, &mfx_ver, &decode.mfx_session);
    if (err != MFX_ERR_NONE) {
        fprintf(stderr, "Error initializing an MFX session\n");
        goto finish;
    }
    MFXVideoCORE_SetHandle(decode.mfx_session, MFX_HANDLE_VA_DISPLAY, decode.va_dpy);
    MFXVideoCORE_SetFrameAllocator(decode.mfx_session, &frame_allocator);
    
    decoder = avcodec_find_decoder_by_name("h264_qsv");
    if (!decoder) {
        fprintf(stderr, "The QSV decoder is not present in libavcodec\n");
        goto finish;
    }
    decoder_ctx = avcodec_alloc_context3(decoder);
    if (!decoder_ctx) {
        ret = AVERROR(ENOMEM);
        goto finish;
    }
    decoder_ctx->codec_id = AV_CODEC_ID_H264;
    if (video_st->codec->extradata_size) {
        decoder_ctx->extradata = av_mallocz(video_st->codec->extradata_size +
                                            AV_INPUT_BUFFER_PADDING_SIZE);
        if (!decoder_ctx->extradata) {
            ret = AVERROR(ENOMEM);
            goto finish;
        }
        memcpy(decoder_ctx->extradata, video_st->codec->extradata,
               video_st->codec->extradata_size);
        decoder_ctx->extradata_size = video_st->codec->extradata_size;
    }
    decoder_ctx->refcounted_frames = 1;
    decoder_ctx->opaque      = &decode;
    decoder_ctx->get_buffer2 = get_buffer;
    decoder_ctx->get_format  = get_format;
    ret = avcodec_open2(decoder_ctx, NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error opening the decoder: ");
        goto finish;
    }
    
    ret = avio_open(&output_ctx, argv[2], AVIO_FLAG_WRITE);
    if (ret < 0) {
        fprintf(stderr, "Error opening the output context: ");
        goto finish;
    }
    frame = av_frame_alloc();
    if (!frame) {
        ret = AVERROR(ENOMEM);
        goto finish;
    }
    
    while (ret >= 0) {
        ret = av_read_frame(input_ctx, &pkt);
        if (ret < 0)
            break;
        if (pkt.stream_index == video_st->index)
            ret = decode_packet(&decode, decoder_ctx, frame, &pkt, output_ctx);
        av_packet_unref(&pkt);
    }
    
    pkt.data = NULL;
    pkt.size = 0;
    ret = decode_packet(&decode, decoder_ctx, frame, &pkt, output_ctx);
finish:
    if (ret < 0) {
        char buf[1024];
        av_strerror(ret, buf, sizeof(buf));
        fprintf(stderr, "%s\n", buf);
    }
    avformat_close_input(&input_ctx);
    av_frame_free(&frame);
    if (decode.mfx_session)
        MFXClose(decode.mfx_session);
    if (decode.va_dpy)
        vaTerminate(decode.va_dpy);
    if (dpy)
        XCloseDisplay(dpy);
    if (decoder_ctx)
        av_freep(&decoder_ctx->hwaccel_context);
    avcodec_free_context(&decoder_ctx);
    avio_close(output_ctx);
    return ret;
}
