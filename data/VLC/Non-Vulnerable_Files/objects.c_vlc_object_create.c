 */
void *vlc_object_create( vlc_object_t *p_this, size_t i_size )
{
    return vlc_custom_create( p_this, i_size, "generic" );
}
