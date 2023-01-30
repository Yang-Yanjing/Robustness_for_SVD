}
int BufferDestroy( buffer_t *p_buffer )
{
    free( p_buffer->p_memory );
    p_buffer->p_memory = NULL;
    p_buffer->p_begin = NULL;
    return VLC_SUCCESS;
}
