}
static long STDCALL GetMaxLength( IMediaBuffer *This, uint32_t *pcbMaxLength )
{
    CMediaBuffer *p_mb = (CMediaBuffer *)This;
    if( !pcbMaxLength ) return E_POINTER;
    *pcbMaxLength = p_mb->i_max_size;
    return S_OK;
}
