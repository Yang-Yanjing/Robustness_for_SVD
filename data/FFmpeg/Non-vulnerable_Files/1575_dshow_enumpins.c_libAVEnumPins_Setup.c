static int
libAVEnumPins_Setup(libAVEnumPins *this, libAVPin *pin, libAVFilter *filter)
{
    IEnumPinsVtbl *vtbl = this->vtbl;
    SETVTBL(vtbl, libAVEnumPins, QueryInterface);
    SETVTBL(vtbl, libAVEnumPins, AddRef);
    SETVTBL(vtbl, libAVEnumPins, Release);
    SETVTBL(vtbl, libAVEnumPins, Next);
    SETVTBL(vtbl, libAVEnumPins, Skip);
    SETVTBL(vtbl, libAVEnumPins, Reset);
    SETVTBL(vtbl, libAVEnumPins, Clone);
    this->pin = pin;
    this->filter = filter;
    libAVFilter_AddRef(this->filter);
    return 1;
}
