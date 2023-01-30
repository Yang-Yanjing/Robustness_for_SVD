static int
dshow_open_device(AVFormatContext *avctx, ICreateDevEnum *devenum,
                  enum dshowDeviceType devtype, enum dshowSourceFilterType sourcetype)
{
    struct dshow_ctx *ctx = avctx->priv_data;
    IBaseFilter *device_filter = NULL;
    IGraphBuilder *graph = ctx->graph;
    IPin *device_pin = NULL;
    libAVPin *capture_pin = NULL;
    libAVFilter *capture_filter = NULL;
    ICaptureGraphBuilder2 *graph_builder2 = NULL;
    int ret = AVERROR(EIO);
    int r;
    IStream *ifile_stream = NULL;
    IStream *ofile_stream = NULL;
    IPersistStream *pers_stream = NULL;
    const wchar_t *filter_name[2] = { L"Audio capture filter", L"Video capture filter" };
    if ( ((ctx->audio_filter_load_file) && (strlen(ctx->audio_filter_load_file)>0) && (sourcetype == AudioSourceDevice)) ||
            ((ctx->video_filter_load_file) && (strlen(ctx->video_filter_load_file)>0) && (sourcetype == VideoSourceDevice)) ) {
        HRESULT hr;
        char *filename = NULL;
        if (sourcetype == AudioSourceDevice)
            filename = ctx->audio_filter_load_file;
        else
            filename = ctx->video_filter_load_file;
        hr = SHCreateStreamOnFile ((LPCSTR) filename, STGM_READ, &ifile_stream);
        if (S_OK != hr) {
            av_log(avctx, AV_LOG_ERROR, "Could not open capture filter description file.\n");
            goto error;
        }
        hr = OleLoadFromStream(ifile_stream, &IID_IBaseFilter, (void **) &device_filter);
        if (hr != S_OK) {
            av_log(avctx, AV_LOG_ERROR, "Could not load capture filter from file.\n");
            goto error;
        }
        if (sourcetype == AudioSourceDevice)
            av_log(avctx, AV_LOG_INFO, "Audio-");
        else
            av_log(avctx, AV_LOG_INFO, "Video-");
        av_log(avctx, AV_LOG_INFO, "Capture filter loaded successfully from file \"%s\".\n", filename);
    } else {
        if ((r = dshow_cycle_devices(avctx, devenum, devtype, sourcetype, &device_filter)) < 0) {
            ret = r;
            goto error;
        }
    }
    ctx->device_filter [devtype] = device_filter;
    r = IGraphBuilder_AddFilter(graph, device_filter, NULL);
    if (r != S_OK) {
        av_log(avctx, AV_LOG_ERROR, "Could not add device filter to graph.\n");
        goto error;
    }
    if ((r = dshow_cycle_pins(avctx, devtype, sourcetype, device_filter, &device_pin)) < 0) {
        ret = r;
        goto error;
    }
    ctx->device_pin[devtype] = device_pin;
    capture_filter = libAVFilter_Create(avctx, callback, devtype);
    if (!capture_filter) {
        av_log(avctx, AV_LOG_ERROR, "Could not create grabber filter.\n");
        goto error;
    }
    ctx->capture_filter[devtype] = capture_filter;
    if ( ((ctx->audio_filter_save_file) && (strlen(ctx->audio_filter_save_file)>0) && (sourcetype == AudioSourceDevice)) ||
            ((ctx->video_filter_save_file) && (strlen(ctx->video_filter_save_file)>0) && (sourcetype == VideoSourceDevice)) ) {
        HRESULT hr;
        char *filename = NULL;
        if (sourcetype == AudioSourceDevice)
            filename = ctx->audio_filter_save_file;
        else
            filename = ctx->video_filter_save_file;
        hr = SHCreateStreamOnFile ((LPCSTR) filename, STGM_CREATE | STGM_READWRITE, &ofile_stream);
        if (S_OK != hr) {
            av_log(avctx, AV_LOG_ERROR, "Could not create capture filter description file.\n");
            goto error;
        }
        hr  = IBaseFilter_QueryInterface(device_filter, &IID_IPersistStream, (void **) &pers_stream);
        if (hr != S_OK) {
            av_log(avctx, AV_LOG_ERROR, "Query for IPersistStream failed.\n");
            goto error;
        }
        hr = OleSaveToStream(pers_stream, ofile_stream);
        if (hr != S_OK) {
            av_log(avctx, AV_LOG_ERROR, "Could not save capture filter \n");
            goto error;
        }
        hr = IStream_Commit(ofile_stream, STGC_DEFAULT);
        if (S_OK != hr) {
            av_log(avctx, AV_LOG_ERROR, "Could not commit capture filter data to file.\n");
            goto error;
        }
        if (sourcetype == AudioSourceDevice)
            av_log(avctx, AV_LOG_INFO, "Audio-");
        else
            av_log(avctx, AV_LOG_INFO, "Video-");
        av_log(avctx, AV_LOG_INFO, "Capture filter saved successfully to file \"%s\".\n", filename);
    }
    r = IGraphBuilder_AddFilter(graph, (IBaseFilter *) capture_filter,
                                filter_name[devtype]);
    if (r != S_OK) {
        av_log(avctx, AV_LOG_ERROR, "Could not add capture filter to graph\n");
        goto error;
    }
    libAVPin_AddRef(capture_filter->pin);
    capture_pin = capture_filter->pin;
    ctx->capture_pin[devtype] = capture_pin;
    r = CoCreateInstance(&CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
                         &IID_ICaptureGraphBuilder2, (void **) &graph_builder2);
    if (r != S_OK) {
        av_log(avctx, AV_LOG_ERROR, "Could not create CaptureGraphBuilder2\n");
        goto error;
    }
    ICaptureGraphBuilder2_SetFiltergraph(graph_builder2, graph);
    if (r != S_OK) {
        av_log(avctx, AV_LOG_ERROR, "Could not set graph for CaptureGraphBuilder2\n");
        goto error;
    }
    r = ICaptureGraphBuilder2_RenderStream(graph_builder2, NULL, NULL, (IUnknown *) device_pin, NULL ,
        (IBaseFilter *) capture_filter); 
    if (r != S_OK) {
        av_log(avctx, AV_LOG_ERROR, "Could not RenderStream to connect pins\n");
        goto error;
    }
    r = dshow_try_setup_crossbar_options(graph_builder2, device_filter, devtype, avctx);
    if (r != S_OK) {
        av_log(avctx, AV_LOG_ERROR, "Could not setup CrossBar\n");
        goto error;
    }
    ret = 0;
error:
    if (graph_builder2 != NULL)
        ICaptureGraphBuilder2_Release(graph_builder2);
    if (pers_stream)
        IPersistStream_Release(pers_stream);
    if (ifile_stream)
        IStream_Release(ifile_stream);
    if (ofile_stream)
        IStream_Release(ofile_stream);
    return ret;
}
