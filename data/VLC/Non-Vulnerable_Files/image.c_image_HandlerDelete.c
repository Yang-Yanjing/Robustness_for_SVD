 */
void image_HandlerDelete( image_handler_t *p_image )
{
    if( !p_image ) return;
    if( p_image->p_dec ) DeleteDecoder( p_image->p_dec );
    if( p_image->p_enc ) DeleteEncoder( p_image->p_enc );
    if( p_image->p_filter ) DeleteFilter( p_image->p_filter );
    free( p_image );
    p_image = NULL;
}
