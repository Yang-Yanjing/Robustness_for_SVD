 *****************************************************************************/
int BufferInit( buffer_t *p_buffer )
{
    memset( p_buffer, 0, sizeof( buffer_t ) );
    p_buffer->p_memory = NULL;
    p_buffer->p_begin = NULL;
    return VLC_SUCCESS;
}
