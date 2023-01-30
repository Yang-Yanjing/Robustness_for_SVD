}
static void bo_AddDWordBE( buffer_out_t *p_bo, uint32_t i )
{
    bo_AddWordBE( p_bo, ( ( i >> 16) &0xffff ) );
    bo_AddWordBE( p_bo, i &0xffff );
}
