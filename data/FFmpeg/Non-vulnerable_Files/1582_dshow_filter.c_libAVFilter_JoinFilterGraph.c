long WINAPI
libAVFilter_JoinFilterGraph(libAVFilter *this, IFilterGraph *graph,
                            const wchar_t *name)
{
    dshowdebug("libAVFilter_JoinFilterGraph(%p)\n", this);
    this->info.pGraph = graph;
    if (name)
        wcscpy(this->info.achName, name);
    return S_OK;
}
