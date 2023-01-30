 */
image_handler_t *image_HandlerCreate( vlc_object_t *p_this )
{
    image_handler_t *p_image = calloc( 1, sizeof(image_handler_t) );
    if( !p_image )
        return NULL;
    p_image->p_parent = p_this;
    p_image->pf_read = ImageRead;
    p_image->pf_read_url = ImageReadUrl;
    p_image->pf_write = ImageWrite;
    p_image->pf_write_url = ImageWriteUrl;
    p_image->pf_convert = ImageConvert;
    p_image->pf_filter = ImageFilter;
    return p_image;
}
