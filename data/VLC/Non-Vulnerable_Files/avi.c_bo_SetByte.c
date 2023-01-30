}
static void bo_SetByte( buffer_out_t *p_bo, int i_offset, uint8_t i )
{
    if( i_offset >= p_bo->i_buffer_size )
    {
        int i_growth = HDR_BASE_SIZE;
        while( i_offset >= p_bo->i_buffer_size + i_growth )
        {
            i_growth += HDR_BASE_SIZE;
        }
        int i = p_bo->p_block->i_buffer; /* Realloc would set payload size == buffer size */
        p_bo->p_block = block_Realloc( p_bo->p_block, 0, p_bo->i_buffer_size + i_growth );
        p_bo->p_block->i_buffer = i;
        p_bo->i_buffer_size += i_growth;
    }
    p_bo->p_block->p_buffer[i_offset] = i;
}
