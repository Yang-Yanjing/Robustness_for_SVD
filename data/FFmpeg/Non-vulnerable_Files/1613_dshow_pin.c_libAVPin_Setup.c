static int
libAVPin_Setup(libAVPin *this, libAVFilter *filter)
{
    IPinVtbl *vtbl = this->vtbl;
    IMemInputPinVtbl *imemvtbl;
    if (!filter)
        return 0;
    imemvtbl = av_malloc(sizeof(IMemInputPinVtbl));
    if (!imemvtbl)
        return 0;
    SETVTBL(imemvtbl, libAVMemInputPin, QueryInterface);
    SETVTBL(imemvtbl, libAVMemInputPin, AddRef);
    SETVTBL(imemvtbl, libAVMemInputPin, Release);
    SETVTBL(imemvtbl, libAVMemInputPin, GetAllocator);
    SETVTBL(imemvtbl, libAVMemInputPin, NotifyAllocator);
    SETVTBL(imemvtbl, libAVMemInputPin, GetAllocatorRequirements);
    SETVTBL(imemvtbl, libAVMemInputPin, Receive);
    SETVTBL(imemvtbl, libAVMemInputPin, ReceiveMultiple);
    SETVTBL(imemvtbl, libAVMemInputPin, ReceiveCanBlock);
    this->imemvtbl = imemvtbl;
    SETVTBL(vtbl, libAVPin, QueryInterface);
    SETVTBL(vtbl, libAVPin, AddRef);
    SETVTBL(vtbl, libAVPin, Release);
    SETVTBL(vtbl, libAVPin, Connect);
    SETVTBL(vtbl, libAVPin, ReceiveConnection);
    SETVTBL(vtbl, libAVPin, Disconnect);
    SETVTBL(vtbl, libAVPin, ConnectedTo);
    SETVTBL(vtbl, libAVPin, ConnectionMediaType);
    SETVTBL(vtbl, libAVPin, QueryPinInfo);
    SETVTBL(vtbl, libAVPin, QueryDirection);
    SETVTBL(vtbl, libAVPin, QueryId);
    SETVTBL(vtbl, libAVPin, QueryAccept);
    SETVTBL(vtbl, libAVPin, EnumMediaTypes);
    SETVTBL(vtbl, libAVPin, QueryInternalConnections);
    SETVTBL(vtbl, libAVPin, EndOfStream);
    SETVTBL(vtbl, libAVPin, BeginFlush);
    SETVTBL(vtbl, libAVPin, EndFlush);
    SETVTBL(vtbl, libAVPin, NewSegment);
    this->filter = filter;
    return 1;
}
