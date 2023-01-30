}
static HRESULT StreamPause( aout_stream_t *s, bool pause )
{
    return Pause( s->sys, pause );
}
