}
static HRESULT StreamPlay( aout_stream_t *s, block_t *block )
{
    return Play( VLC_OBJECT(s), s->sys, block );
}
