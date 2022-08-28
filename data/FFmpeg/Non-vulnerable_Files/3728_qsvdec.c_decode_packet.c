static int decode_packet(DecodeContext *decode, AVCodecContext *decoder_ctx,
                         AVFrame *frame, AVPacket *pkt,
                         AVIOContext *output_ctx)
{
    int ret = 0;
    int got_frame = 1;
    while (pkt->size > 0 || (!pkt->data && got_frame)) {
        ret = avcodec_decode_video2(decoder_ctx, frame, &got_frame, pkt);
        if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            return ret;
        }
        pkt->data += ret;
        pkt->size -= ret;
        


        if (got_frame) {
            mfxFrameSurface1 *surf = (mfxFrameSurface1*)frame->data[3];
            VASurfaceID    surface = *(VASurfaceID*)surf->Data.MemId;
            VAImageFormat img_fmt = {
                .fourcc         = VA_FOURCC_NV12,
                .byte_order     = VA_LSB_FIRST,
                .bits_per_pixel = 8,
                .depth          = 8,
            };
            VAImage img;
            VAStatus err;
            uint8_t *data;
            int i, j;
            img.buf      = VA_INVALID_ID;
            img.image_id = VA_INVALID_ID;
            err = vaCreateImage(decode->va_dpy, &img_fmt,
                                frame->width, frame->height, &img);
            if (err != VA_STATUS_SUCCESS) {
                fprintf(stderr, "Error creating an image: %s\n",
                        vaErrorStr(err));
                ret = AVERROR_UNKNOWN;
                goto fail;
            }
            err = vaGetImage(decode->va_dpy, surface, 0, 0,
                             frame->width, frame->height,
                             img.image_id);
            if (err != VA_STATUS_SUCCESS) {
                fprintf(stderr, "Error getting an image: %s\n",
                        vaErrorStr(err));
                ret = AVERROR_UNKNOWN;
                goto fail;
            }
            err = vaMapBuffer(decode->va_dpy, img.buf, (void**)&data);
            if (err != VA_STATUS_SUCCESS) {
                fprintf(stderr, "Error mapping the image buffer: %s\n",
                        vaErrorStr(err));
                ret = AVERROR_UNKNOWN;
                goto fail;
            }
            for (i = 0; i < img.num_planes; i++)
                for (j = 0; j < (img.height >> (i > 0)); j++)
                    avio_write(output_ctx, data + img.offsets[i] + j * img.pitches[i], img.width);
fail:
            if (img.buf != VA_INVALID_ID)
                vaUnmapBuffer(decode->va_dpy, img.buf);
            if (img.image_id != VA_INVALID_ID)
                vaDestroyImage(decode->va_dpy, img.image_id);
            av_frame_unref(frame);
            if (ret < 0)
                return ret;
        }
    }
    return 0;
}
