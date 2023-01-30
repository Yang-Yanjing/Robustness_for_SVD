long WINAPI
libAVEnumPins_Reset(libAVEnumPins *this)
{
    dshowdebug("libAVEnumPins_Reset(%p)\n", this);
    this->pos = 0;
    return S_OK;
}
