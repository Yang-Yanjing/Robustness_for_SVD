}
static long STDCALL AddRef( IUnknown *This )
{
    CMediaBuffer *p_mb = (CMediaBuffer *)This;
    return p_mb->i_ref++;
}
