void
libAVMemInputPin_Destroy(libAVMemInputPin *this)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    dshowdebug("libAVMemInputPin_Destroy(%p)\n", this);
    libAVPin_Destroy(pin);
}
