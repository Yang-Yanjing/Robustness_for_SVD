}
block_t *block_Realloc( block_t *p_block, ssize_t i_prebody, size_t i_body )
{
    size_t requested = i_prebody + i_body;
    block_Check( p_block );
    /* Corner case: empty block requested */
    if( i_prebody <= 0 && i_body <= (size_t)(-i_prebody) )
    {
        block_Release( p_block );
        return NULL;
    }
    assert( p_block->p_start <= p_block->p_buffer );
    assert( p_block->p_start + p_block->i_size
                                    >= p_block->p_buffer + p_block->i_buffer );
    /* Corner case: the current payload is discarded completely */
    if( i_prebody <= 0 && p_block->i_buffer <= (size_t)-i_prebody )
         p_block->i_buffer = 0; /* discard current payload */
    if( p_block->i_buffer == 0 )
    {
        if( requested <= p_block->i_size )
        {   /* Enough room: recycle buffer */
            size_t extra = p_block->i_size - requested;
            p_block->p_buffer = p_block->p_start + (extra / 2);
            p_block->i_buffer = requested;
            return p_block;
        }
        /* Not enough room: allocate a new buffer */
        block_t *p_rea = block_Alloc( requested );
        if( p_rea )
            BlockMetaCopy( p_rea, p_block );
        block_Release( p_block );
        return p_rea;
    }
    /* First, shrink payload */
    /* Pull payload start */
    if( i_prebody < 0 )
    {
        assert( p_block->i_buffer >= (size_t)-i_prebody );
        p_block->p_buffer -= i_prebody;
        p_block->i_buffer += i_prebody;
        i_body += i_prebody;
        i_prebody = 0;
    }
    /* Trim payload end */
    if( p_block->i_buffer > i_body )
        p_block->i_buffer = i_body;
    uint8_t *p_start = p_block->p_start;
    uint8_t *p_end = p_start + p_block->i_size;
    /* Second, reallocate the buffer if we lack space. This is done now to
     * minimize the payload size for memory copy. */
    assert( i_prebody >= 0 );
    if( (size_t)(p_block->p_buffer - p_start) < (size_t)i_prebody
     || (size_t)(p_end - p_block->p_buffer) < i_body )
    {
        block_t *p_rea = block_Alloc( requested );
        if( p_rea )
        {
            BlockMetaCopy( p_rea, p_block );
            p_rea->p_buffer += i_prebody;
            p_rea->i_buffer -= i_prebody;
            memcpy( p_rea->p_buffer, p_block->p_buffer, p_block->i_buffer );
        }
        block_Release( p_block );
        if( p_rea == NULL )
            return NULL;
        p_block = p_rea;
    }
    else
    /* We have a very large reserved footer now? Release some of it.
     * XXX it might not preserve the alignment of p_buffer */
    if( p_end - (p_block->p_buffer + i_body) > BLOCK_WASTE_SIZE )
    {
        block_t *p_rea = block_Alloc( requested );
        if( p_rea )
        {
            BlockMetaCopy( p_rea, p_block );
            p_rea->p_buffer += i_prebody;
            p_rea->i_buffer -= i_prebody;
            memcpy( p_rea->p_buffer, p_block->p_buffer, p_block->i_buffer );
            block_Release( p_block );
            p_block = p_rea;
        }
    }
    /* NOTE: p_start and p_end are corrupted from this point */
    /* Third, expand payload */
    /* Push payload start */
    if( i_prebody > 0 )
    {
        p_block->p_buffer -= i_prebody;
        p_block->i_buffer += i_prebody;
        i_body += i_prebody;
        i_prebody = 0;
    }
    /* Expand payload to requested size */
    p_block->i_buffer = i_body;
    return p_block;
}
