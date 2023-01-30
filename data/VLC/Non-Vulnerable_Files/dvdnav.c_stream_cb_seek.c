 *****************************************************************************/
static int stream_cb_seek( void *s, uint64_t pos )
{
    return stream_Seek( (stream_t *)s, pos );
}
