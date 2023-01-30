}
void bd_clpi_Clean( bd_clpi_t *p_clpi )
{
    free( p_clpi->p_stc );
    free( p_clpi->p_stream );
    for( int i = 0; i < p_clpi->i_ep_map; i++ )
        bd_clpi_ep_map_Clean( &p_clpi->p_ep_map[i] );
    free( p_clpi->p_ep_map );
}
