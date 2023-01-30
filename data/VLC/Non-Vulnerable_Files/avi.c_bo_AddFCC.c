#endif
static void bo_AddFCC( buffer_out_t *p_bo, const char *fcc )
{
    bo_AddByte( p_bo, fcc[0] );
    bo_AddByte( p_bo, fcc[1] );
    bo_AddByte( p_bo, fcc[2] );
    bo_AddByte( p_bo, fcc[3] );
}
