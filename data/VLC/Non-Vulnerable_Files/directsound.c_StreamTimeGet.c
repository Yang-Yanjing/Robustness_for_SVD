}
static HRESULT StreamTimeGet( aout_stream_t *s, mtime_t *delay )
{
    return TimeGet( s->sys, delay );
}
