static int vfw_read_header(AVFormatContext *s)
{
    struct vfw_ctx *ctx = s->priv_data;
    AVCodecContext *codec;
    AVStream *st;
    int devnum;
    int bisize;
    BITMAPINFO *bi = NULL;
    CAPTUREPARMS cparms;
    DWORD biCompression;
    WORD biBitCount;
    int ret;
    AVRational framerate_q;
    if (!strcmp(s->filename, "list")) {
        for (devnum = 0; devnum <= 9; devnum++) {
            char driver_name[256];
            char driver_ver[256];
            ret = capGetDriverDescription(devnum,
                                          driver_name, sizeof(driver_name),
                                          driver_ver, sizeof(driver_ver));
            if (ret) {
                av_log(s, AV_LOG_INFO, "Driver %d\n", devnum);
                av_log(s, AV_LOG_INFO, " %s\n", driver_name);
                av_log(s, AV_LOG_INFO, " %s\n", driver_ver);
            }
        }
        return AVERROR(EIO);
    }
    ctx->hwnd = capCreateCaptureWindow(NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, 0);
    if(!ctx->hwnd) {
        av_log(s, AV_LOG_ERROR, "Could not create capture window.\n");
        return AVERROR(EIO);
    }
    
    devnum = atoi(s->filename);
    ret = SendMessage(ctx->hwnd, WM_CAP_DRIVER_CONNECT, devnum, 0);
    if(!ret) {
        av_log(s, AV_LOG_ERROR, "Could not connect to device.\n");
        DestroyWindow(ctx->hwnd);
        return AVERROR(ENODEV);
    }
    SendMessage(ctx->hwnd, WM_CAP_SET_OVERLAY, 0, 0);
    SendMessage(ctx->hwnd, WM_CAP_SET_PREVIEW, 0, 0);
    ret = SendMessage(ctx->hwnd, WM_CAP_SET_CALLBACK_VIDEOSTREAM, 0,
                      (LPARAM) videostream_cb);
    if(!ret) {
        av_log(s, AV_LOG_ERROR, "Could not set video stream callback.\n");
        goto fail;
    }
    SetWindowLongPtr(ctx->hwnd, GWLP_USERDATA, (LONG_PTR) s);
    st = avformat_new_stream(s, NULL);
    if(!st) {
        vfw_read_close(s);
        return AVERROR(ENOMEM);
    }
    
    bisize = SendMessage(ctx->hwnd, WM_CAP_GET_VIDEOFORMAT, 0, 0);
    if(!bisize)
        goto fail;
    bi = av_malloc(bisize);
    if(!bi) {
        vfw_read_close(s);
        return AVERROR(ENOMEM);
    }
    ret = SendMessage(ctx->hwnd, WM_CAP_GET_VIDEOFORMAT, bisize, (LPARAM) bi);
    if(!ret)
        goto fail;
    dump_bih(s, &bi->bmiHeader);
    ret = av_parse_video_rate(&framerate_q, ctx->framerate);
    if (ret < 0) {
        av_log(s, AV_LOG_ERROR, "Could not parse framerate '%s'.\n", ctx->framerate);
        goto fail;
    }
    if (ctx->video_size) {
        ret = av_parse_video_size(&bi->bmiHeader.biWidth, &bi->bmiHeader.biHeight, ctx->video_size);
        if (ret < 0) {
            av_log(s, AV_LOG_ERROR, "Couldn't parse video size.\n");
            goto fail;
        }
    }
    if (0) {
        

        bi->bmiHeader.biWidth       = 320;
        bi->bmiHeader.biHeight      = 240;
        bi->bmiHeader.biPlanes      = 1;
        bi->bmiHeader.biBitCount    = 12;
        bi->bmiHeader.biCompression = MKTAG('I','4','2','0');
        bi->bmiHeader.biSizeImage   = 115200;
        dump_bih(s, &bi->bmiHeader);
    }
    ret = SendMessage(ctx->hwnd, WM_CAP_SET_VIDEOFORMAT, bisize, (LPARAM) bi);
    if(!ret) {
        av_log(s, AV_LOG_ERROR, "Could not set Video Format.\n");
        goto fail;
    }
    biCompression = bi->bmiHeader.biCompression;
    biBitCount = bi->bmiHeader.biBitCount;
    
    ret = SendMessage(ctx->hwnd, WM_CAP_GET_SEQUENCE_SETUP, sizeof(cparms),
                      (LPARAM) &cparms);
    if(!ret)
        goto fail;
    dump_captureparms(s, &cparms);
    cparms.fYield = 1; 
    cparms.dwRequestMicroSecPerFrame =
                               (framerate_q.den*1000000) / framerate_q.num;
    cparms.fAbortLeftMouse = 0;
    cparms.fAbortRightMouse = 0;
    cparms.fCaptureAudio = 0;
    cparms.vKeyAbort = 0;
    ret = SendMessage(ctx->hwnd, WM_CAP_SET_SEQUENCE_SETUP, sizeof(cparms),
                      (LPARAM) &cparms);
    if(!ret)
        goto fail;
    codec = st->codec;
    codec->time_base = av_inv_q(framerate_q);
    codec->codec_type = AVMEDIA_TYPE_VIDEO;
    codec->width  = bi->bmiHeader.biWidth;
    codec->height = bi->bmiHeader.biHeight;
    codec->pix_fmt = vfw_pixfmt(biCompression, biBitCount);
    if(codec->pix_fmt == AV_PIX_FMT_NONE) {
        codec->codec_id = vfw_codecid(biCompression);
        if(codec->codec_id == AV_CODEC_ID_NONE) {
            av_log(s, AV_LOG_ERROR, "Unknown compression type. "
                             "Please report verbose (-v 9) debug information.\n");
            vfw_read_close(s);
            return AVERROR_PATCHWELCOME;
        }
        codec->bits_per_coded_sample = biBitCount;
    } else {
        codec->codec_id = AV_CODEC_ID_RAWVIDEO;
        if(biCompression == BI_RGB) {
            codec->bits_per_coded_sample = biBitCount;
            codec->extradata = av_malloc(9 + AV_INPUT_BUFFER_PADDING_SIZE);
            if (codec->extradata) {
                codec->extradata_size = 9;
                memcpy(codec->extradata, "BottomUp", 9);
            }
        }
    }
    av_freep(&bi);
    avpriv_set_pts_info(st, 32, 1, 1000);
    ctx->mutex = CreateMutex(NULL, 0, NULL);
    if(!ctx->mutex) {
        av_log(s, AV_LOG_ERROR, "Could not create Mutex.\n" );
        goto fail;
    }
    ctx->event = CreateEvent(NULL, 1, 0, NULL);
    if(!ctx->event) {
        av_log(s, AV_LOG_ERROR, "Could not create Event.\n" );
        goto fail;
    }
    ret = SendMessage(ctx->hwnd, WM_CAP_SEQUENCE_NOFILE, 0, 0);
    if(!ret) {
        av_log(s, AV_LOG_ERROR, "Could not start capture sequence.\n" );
        goto fail;
    }
    return 0;
fail:
    av_freep(&bi);
    vfw_read_close(s);
    return AVERROR(EIO);
}
