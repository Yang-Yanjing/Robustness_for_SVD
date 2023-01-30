}
static void bo_AddWordBE( buffer_out_t *p_bo, uint16_t i )
{
    bo_AddByte( p_bo, ( ( i >> 8) &0xff ) );
    bo_AddByte( p_bo, i &0xff );
}
