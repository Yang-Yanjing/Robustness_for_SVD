static int
libAVFilter_Setup(libAVFilter *this, void *priv_data, void *callback,
                  enum dshowDeviceType type)
{
    IBaseFilterVtbl *vtbl = this->vtbl;
    SETVTBL(vtbl, libAVFilter, QueryInterface);
    SETVTBL(vtbl, libAVFilter, AddRef);
    SETVTBL(vtbl, libAVFilter, Release);
    SETVTBL(vtbl, libAVFilter, GetClassID);
    SETVTBL(vtbl, libAVFilter, Stop);
    SETVTBL(vtbl, libAVFilter, Pause);
    SETVTBL(vtbl, libAVFilter, Run);
    SETVTBL(vtbl, libAVFilter, GetState);
    SETVTBL(vtbl, libAVFilter, SetSyncSource);
    SETVTBL(vtbl, libAVFilter, GetSyncSource);
    SETVTBL(vtbl, libAVFilter, EnumPins);
    SETVTBL(vtbl, libAVFilter, FindPin);
    SETVTBL(vtbl, libAVFilter, QueryFilterInfo);
    SETVTBL(vtbl, libAVFilter, JoinFilterGraph);
    SETVTBL(vtbl, libAVFilter, QueryVendorInfo);
    this->pin = libAVPin_Create(this);
    this->priv_data = priv_data;
    this->callback  = callback;
    this->type      = type;
    return 1;
}
