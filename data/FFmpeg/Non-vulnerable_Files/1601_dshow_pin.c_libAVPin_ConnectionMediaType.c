long WINAPI
libAVPin_ConnectionMediaType(libAVPin *this, AM_MEDIA_TYPE *type)
{
    dshowdebug("libAVPin_ConnectionMediaType(%p)\n", this);
    if (!type)
        return E_POINTER;
    if (!this->connectedto)
        return VFW_E_NOT_CONNECTED;
    return ff_copy_dshow_media_type(type, &this->type);
}
