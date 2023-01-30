} buffer_out_t;
static void bo_Init( buffer_out_t *p_bo, int i_size )
{
    p_bo->p_block = block_Alloc( i_size );
    p_bo->p_block->i_buffer = 0;
    p_bo->i_buffer_size = i_size;
}
