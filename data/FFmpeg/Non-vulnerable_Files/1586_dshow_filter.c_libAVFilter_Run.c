long WINAPI
libAVFilter_Run(libAVFilter *this, REFERENCE_TIME start)
{
    dshowdebug("libAVFilter_Run(%p) %"PRId64"\n", this, start);
    this->state = State_Running;
    this->start_time = start;
    return S_OK;
}
