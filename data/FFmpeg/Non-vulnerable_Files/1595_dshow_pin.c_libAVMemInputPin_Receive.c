long WINAPI
libAVMemInputPin_Receive(libAVMemInputPin *this, IMediaSample *sample)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    enum dshowDeviceType devtype = pin->filter->type;
    void *priv_data;
    AVFormatContext *s;
    uint8_t *buf;
    int buf_size; 
    int index;
    int64_t curtime;
    int64_t orig_curtime;
    int64_t graphtime;
    const char *devtypename = (devtype == VideoDevice) ? "video" : "audio";
    IReferenceClock *clock = pin->filter->clock;
    int64_t dummy;
    struct dshow_ctx *ctx;
    dshowdebug("libAVMemInputPin_Receive(%p)\n", this);
    if (!sample)
        return E_POINTER;
    IMediaSample_GetTime(sample, &orig_curtime, &dummy);
    orig_curtime += pin->filter->start_time;
    IReferenceClock_GetTime(clock, &graphtime);
    if (devtype == VideoDevice) {
        
        IReferenceClock_GetTime(clock, &curtime);
    } else {
        IMediaSample_GetTime(sample, &curtime, &dummy);
        if(curtime > 400000000000000000LL) {
            


            av_log(NULL, AV_LOG_DEBUG,
                "dshow dropping initial (or ending) audio frame with odd PTS too high %"PRId64"\n", curtime);
            return S_OK;
        }
        curtime += pin->filter->start_time;
    }
    buf_size = IMediaSample_GetActualDataLength(sample);
    IMediaSample_GetPointer(sample, &buf);
    priv_data = pin->filter->priv_data;
    s = priv_data;
    ctx = s->priv_data;
    index = pin->filter->stream_index;
    av_log(NULL, AV_LOG_VERBOSE, "dshow passing through packet of type %s size %8d "
        "timestamp %"PRId64" orig timestamp %"PRId64" graph timestamp %"PRId64" diff %"PRId64" %s\n",
        devtypename, buf_size, curtime, orig_curtime, graphtime, graphtime - orig_curtime, ctx->device_name[devtype]);
    pin->filter->callback(priv_data, index, buf, buf_size, curtime, devtype);
    return S_OK;
}
