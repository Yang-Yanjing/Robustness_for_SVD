long WINAPI
libAVPin_QueryAccept(libAVPin *this, const AM_MEDIA_TYPE *type)
{
    dshowdebug("libAVPin_QueryAccept(%p)\n", this);
    return S_FALSE;
}
