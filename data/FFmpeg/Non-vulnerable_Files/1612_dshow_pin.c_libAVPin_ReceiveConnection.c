long WINAPI
libAVPin_ReceiveConnection(libAVPin *this, IPin *pin,
                           const AM_MEDIA_TYPE *type)
{
    enum dshowDeviceType devtype = this->filter->type;
    dshowdebug("libAVPin_ReceiveConnection(%p)\n", this);
    if (!pin)
        return E_POINTER;
    if (this->connectedto)
        return VFW_E_ALREADY_CONNECTED;
    ff_print_AM_MEDIA_TYPE(type);
    if (devtype == VideoDevice) {
        if (!IsEqualGUID(&type->majortype, &MEDIATYPE_Video))
            return VFW_E_TYPE_NOT_ACCEPTED;
    } else {
        if (!IsEqualGUID(&type->majortype, &MEDIATYPE_Audio))
            return VFW_E_TYPE_NOT_ACCEPTED;
    }
    IPin_AddRef(pin);
    this->connectedto = pin;
    ff_copy_dshow_media_type(&this->type, type);
    return S_OK;
}
