}
static void bo_AddByte( buffer_out_t *p_bo, uint8_t i )
{
    bo_SetByte( p_bo, p_bo->p_block->i_buffer, i );
    p_bo->p_block->i_buffer++;
}
