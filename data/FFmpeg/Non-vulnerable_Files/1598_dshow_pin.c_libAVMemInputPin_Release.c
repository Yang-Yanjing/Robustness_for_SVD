unsigned long WINAPI
libAVMemInputPin_Release(libAVMemInputPin *this)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    dshowdebug("libAVMemInputPin_Release(%p)\n", this);
    return libAVPin_Release(pin);
}
