}
static HRESULT StreamStop( aout_stream_t *s )
{
    HRESULT hr;
    hr = Stop( s->sys );
    free( s->sys );
    return hr;
}
