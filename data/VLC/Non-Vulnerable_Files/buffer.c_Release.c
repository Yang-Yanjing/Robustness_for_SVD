}
static long STDCALL Release( IUnknown *This )
{
    CMediaBuffer *p_mb = (CMediaBuffer *)This;
    p_mb->i_ref--;
    if( p_mb->i_ref == 0 )
    {
        if( p_mb->b_own ) block_Release( p_mb->p_block );
        free( p_mb->vt );
        free( p_mb );
    }
    return 0;
}
