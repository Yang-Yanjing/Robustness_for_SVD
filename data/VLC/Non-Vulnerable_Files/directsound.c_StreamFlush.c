}
static HRESULT StreamFlush( aout_stream_t *s )
{
    return Flush( s->sys, false );
}
