}
char *BufferGetToken( buffer_t *p_buffer )
{
    char *p_char = p_buffer->p_begin;
    while( isspace( (unsigned char)p_char[0] ) || p_char[0] == '\0' )
    {
        if( p_char <= (p_buffer->p_begin + p_buffer->i_length) )
            p_char++;
        else
            return NULL;
    }
    return p_char;
}
