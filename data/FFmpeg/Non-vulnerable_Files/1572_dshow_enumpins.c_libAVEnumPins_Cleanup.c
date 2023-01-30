static int
libAVEnumPins_Cleanup(libAVEnumPins *this)
{
    libAVFilter_Release(this->filter);
    return 1;
}
