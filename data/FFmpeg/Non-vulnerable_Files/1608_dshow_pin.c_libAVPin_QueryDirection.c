long WINAPI
libAVPin_QueryDirection(libAVPin *this, PIN_DIRECTION *dir)
{
    dshowdebug("libAVPin_QueryDirection(%p)\n", this);
    if (!dir)
        return E_POINTER;
    *dir = PINDIR_INPUT;
    return S_OK;
}
