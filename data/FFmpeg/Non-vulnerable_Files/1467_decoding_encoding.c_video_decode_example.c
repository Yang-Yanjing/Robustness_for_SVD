static void video_decode_example(const char *outfilename, const char *filename)
{
    AVCodec *codec;
    AVCodecContext *c= NULL;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;
    av_init_packet(&avpkt);
    
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    printf("Decode video file %s to %s\n", filename, outfilename);
    
    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED)
        c->flags |= AV_CODEC_FLAG_TRUNCATED; 
    


    
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame_count = 0;
    for (;;) {
        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0)
            break;
        






        


        

        avpkt.data = inbuf;
        while (avpkt.size > 0)
            if (decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 0) < 0)
                exit(1);
    }
    


    avpkt.data = NULL;
    avpkt.size = 0;
    decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 1);
    fclose(f);
    avcodec_close(c);
    av_free(c);
    av_frame_free(&frame);
    printf("\n");
}
