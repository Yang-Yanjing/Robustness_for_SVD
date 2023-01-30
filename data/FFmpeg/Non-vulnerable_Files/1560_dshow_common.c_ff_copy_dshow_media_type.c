long ff_copy_dshow_media_type(AM_MEDIA_TYPE *dst, const AM_MEDIA_TYPE *src)
{
    uint8_t *pbFormat = NULL;
    if (src->cbFormat) {
        pbFormat = CoTaskMemAlloc(src->cbFormat);
        if (!pbFormat)
            return E_OUTOFMEMORY;
        memcpy(pbFormat, src->pbFormat, src->cbFormat);
    }
    *dst = *src;
    dst->pUnk = NULL;
    dst->pbFormat = pbFormat;
    return S_OK;
}
