long WINAPI
libAVFilter_QueryFilterInfo(libAVFilter *this, FILTER_INFO *info)
{
    dshowdebug("libAVFilter_QueryFilterInfo(%p)\n", this);
    if (!info)
        return E_POINTER;
    if (this->info.pGraph)
        IFilterGraph_AddRef(this->info.pGraph);
    *info = this->info;
    return S_OK;
}
