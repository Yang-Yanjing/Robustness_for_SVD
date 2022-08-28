long WINAPI
libAVEnumMediaTypes_Reset(libAVEnumMediaTypes *this)
{
    dshowdebug("libAVEnumMediaTypes_Reset(%p)\n", this);
    this->pos = 0;
    return S_OK;
}
