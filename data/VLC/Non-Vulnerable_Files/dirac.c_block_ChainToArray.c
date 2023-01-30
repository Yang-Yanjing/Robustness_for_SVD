 * member of block_t) */
static int block_ChainToArray( block_t *p_block, block_t ***ppp_array)
{
    if( !ppp_array )
        return 0;
    int i_num_blocks;
    block_ChainProperties( p_block, &i_num_blocks, NULL, NULL );
    *ppp_array = calloc( i_num_blocks, sizeof( block_t* ) );
    if( !*ppp_array ) return 0;
    for( int i = 0; i < i_num_blocks; i++ )
    {
        (*ppp_array)[i] = p_block;
        p_block = p_block->p_next;
    }
    return i_num_blocks;
}
