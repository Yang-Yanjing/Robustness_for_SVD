static int dshow_check_event_queue(IMediaEvent *media_event)
{
    LONG_PTR p1, p2;
    long code;
    int ret = 0;
    while (IMediaEvent_GetEvent(media_event, &code, &p1, &p2, 0) != E_ABORT) {
        if (code == EC_COMPLETE || code == EC_DEVICE_LOST || code == EC_ERRORABORT)
            ret = -1;
        IMediaEvent_FreeEventParams(media_event, code, p1, p2);
    }
    return ret;
}
