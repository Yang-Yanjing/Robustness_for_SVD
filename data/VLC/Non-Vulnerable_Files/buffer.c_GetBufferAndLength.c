}
static long STDCALL GetBufferAndLength( IMediaBuffer *This,
                                        char **ppBuffer, uint32_t *pcbLength )
{
    CMediaBuffer *p_mb = (CMediaBuffer *)This;
    if( !ppBuffer && !pcbLength ) return E_POINTER;
    if( ppBuffer ) *ppBuffer = (char*)p_mb->p_block->p_buffer;
    if( pcbLength ) *pcbLength = p_mb->p_block->i_buffer;
    return S_OK;
}
