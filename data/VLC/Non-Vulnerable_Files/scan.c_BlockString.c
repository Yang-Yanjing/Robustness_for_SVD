}
static block_t *BlockString( const char *psz )
{
    block_t *p = block_Alloc( strlen(psz) );
    if( p )
        memcpy( p->p_buffer, psz, p->i_buffer );
    return p;
}
