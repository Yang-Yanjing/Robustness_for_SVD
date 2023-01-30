}
static long STDCALL SetLength( IMediaBuffer *This, uint32_t cbLength )
{
    CMediaBuffer *p_mb = (CMediaBuffer *)This;
    if( cbLength > (uint32_t)p_mb->i_max_size ) return E_INVALIDARG;
    p_mb->p_block->i_buffer = cbLength;
    return S_OK;
}
