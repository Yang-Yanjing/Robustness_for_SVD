static int
libAVFilter_Cleanup(libAVFilter *this)
{
    libAVPin_Release(this->pin);
    return 1;
}
