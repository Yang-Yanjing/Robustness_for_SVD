}
static void bo_SetDWordLE( buffer_out_t *p_bo, int i_offset, uint32_t i )
{
    bo_SetWordLE( p_bo, i_offset, i &0xffff );
    bo_SetWordLE( p_bo, i_offset + 2, ( ( i >> 16) &0xffff ) );
}
