}
CMediaBuffer *CMediaBufferCreate( block_t *p_block, int i_max_size,
                                  bool b_own )
{
    CMediaBuffer *p_mb = (CMediaBuffer *)malloc( sizeof(CMediaBuffer) );
    if( !p_mb ) return NULL;
    p_mb->vt = (IMediaBuffer_vt *)malloc( sizeof(IMediaBuffer_vt) );
    if( !p_mb->vt )
    {
        free( p_mb );
        return NULL;
    }
    p_mb->i_ref = 1;
    p_mb->p_block = p_block;
    p_mb->i_max_size = i_max_size;
    p_mb->b_own = b_own;
    p_mb->vt->QueryInterface = QueryInterface;
    p_mb->vt->AddRef = AddRef;
    p_mb->vt->Release = Release;
    p_mb->vt->SetLength = SetLength;
    p_mb->vt->GetMaxLength = GetMaxLength;
    p_mb->vt->GetBufferAndLength = GetBufferAndLength;
    return p_mb;
}
