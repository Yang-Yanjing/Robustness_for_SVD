long WINAPI
libAVPin_QueryId(libAVPin *this, wchar_t **id)
{
    dshowdebug("libAVPin_QueryId(%p)\n", this);
    if (!id)
        return E_POINTER;
    *id = wcsdup(L"libAV Pin");
    return S_OK;
}
