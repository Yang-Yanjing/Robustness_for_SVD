unsigned long WINAPI
libAVMemInputPin_AddRef(libAVMemInputPin *this)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    dshowdebug("libAVMemInputPin_AddRef(%p)\n", this);
    return libAVPin_AddRef(pin);
}
