}
static void bo_AddMem( buffer_out_t *p_bo, int i_size, uint8_t *p_mem )
{
    int i;
    for( i = 0; i < i_size; i++ )
    {
        bo_AddByte( p_bo, p_mem[i] );
    }
}
