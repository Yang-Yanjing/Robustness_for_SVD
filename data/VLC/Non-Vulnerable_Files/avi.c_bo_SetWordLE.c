}
static void bo_SetWordLE( buffer_out_t *p_bo, int i_offset, uint16_t i )
{
    bo_SetByte( p_bo, i_offset, i &0xff );
    bo_SetByte( p_bo, i_offset + 1, ( ( i >> 8) &0xff ) );
}
