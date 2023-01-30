char *videotoolbox_pixfmt;
static int videotoolbox_retrieve_data(AVCodecContext *s, AVFrame *frame)
{
    InputStream *ist = s->opaque;
    VTContext  *vt = ist->hwaccel_ctx;
    CVPixelBufferRef pixbuf = (CVPixelBufferRef)frame->data[3];
    OSType pixel_format = CVPixelBufferGetPixelFormatType(pixbuf);
    CVReturn err;
    uint8_t *data[4] = { 0 };
    int linesize[4] = { 0 };
    int planes, ret, i;
    av_frame_unref(vt->tmp_frame);
    switch (pixel_format) {
    case kCVPixelFormatType_420YpCbCr8Planar: vt->tmp_frame->format = AV_PIX_FMT_YUV420P; break;
    case kCVPixelFormatType_422YpCbCr8:       vt->tmp_frame->format = AV_PIX_FMT_UYVY422; break;
    case kCVPixelFormatType_32BGRA:           vt->tmp_frame->format = AV_PIX_FMT_BGRA; break;
#ifdef kCFCoreFoundationVersionNumber10_7
    case kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange: vt->tmp_frame->format = AV_PIX_FMT_NV12; break;
#endif
    default:
        av_log(NULL, AV_LOG_ERROR,
               "%s: Unsupported pixel format: %s\n",
               av_fourcc2str(s->codec_tag), videotoolbox_pixfmt);
        return AVERROR(ENOSYS);
    }
    vt->tmp_frame->width  = frame->width;
    vt->tmp_frame->height = frame->height;
    ret = av_frame_get_buffer(vt->tmp_frame, 32);
    if (ret < 0)
        return ret;
    err = CVPixelBufferLockBaseAddress(pixbuf, kCVPixelBufferLock_ReadOnly);
    if (err != kCVReturnSuccess) {
        av_log(NULL, AV_LOG_ERROR, "Error locking the pixel buffer.\n");
        return AVERROR_UNKNOWN;
    }
    if (CVPixelBufferIsPlanar(pixbuf)) {
        planes = CVPixelBufferGetPlaneCount(pixbuf);
        for (i = 0; i < planes; i++) {
            data[i]     = CVPixelBufferGetBaseAddressOfPlane(pixbuf, i);
            linesize[i] = CVPixelBufferGetBytesPerRowOfPlane(pixbuf, i);
        }
    } else {
        data[0] = CVPixelBufferGetBaseAddress(pixbuf);
        linesize[0] = CVPixelBufferGetBytesPerRow(pixbuf);
    }
    av_image_copy(vt->tmp_frame->data, vt->tmp_frame->linesize,
                  (const uint8_t **)data, linesize, vt->tmp_frame->format,
                  frame->width, frame->height);
    ret = av_frame_copy_props(vt->tmp_frame, frame);
    CVPixelBufferUnlockBaseAddress(pixbuf, kCVPixelBufferLock_ReadOnly);
    if (ret < 0)
        return ret;
    av_frame_unref(frame);
    av_frame_move_ref(frame, vt->tmp_frame);
    return 0;
}
