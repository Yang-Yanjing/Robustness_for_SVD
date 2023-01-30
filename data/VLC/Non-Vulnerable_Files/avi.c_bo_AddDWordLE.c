}
static void bo_AddDWordLE( buffer_out_t *p_bo, uint32_t i )
{
    bo_AddWordLE( p_bo, i &0xffff );
    bo_AddWordLE( p_bo, ( ( i >> 16) &0xffff ) );
}
