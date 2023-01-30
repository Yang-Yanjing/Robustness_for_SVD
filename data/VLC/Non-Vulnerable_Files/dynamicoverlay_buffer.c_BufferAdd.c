}
int BufferAdd( buffer_t *p_buffer, const char *p_data, size_t i_len )
{
    if( ( p_buffer->i_size - p_buffer->i_length -
          ( p_buffer->p_begin - p_buffer->p_memory ) ) < i_len )
    {
        /* We'll have to do some rearranging to fit the new data. */
        if( ( p_buffer->i_size - p_buffer->i_length ) >= i_len )
        {
            /* We have room in the current buffer, just need to move it */
            memmove( p_buffer->p_memory, p_buffer->p_begin,
                     p_buffer->i_length );
            p_buffer->p_begin = p_buffer->p_memory;
        }
        else
        {
            // We need a bigger buffer
            size_t i_newsize = 1024;
            while( i_newsize < p_buffer->i_length + i_len )
                i_newsize *= 2;
            /* TODO: Should I handle wrapping here? */
            /* I'm not using realloc here, as I can avoid a memcpy/memmove in
               some (most?) cases, and reset the start of the buffer. */
            char *p_newdata = malloc( i_newsize );
            if( p_newdata == NULL )
                return VLC_ENOMEM;
            if( p_buffer->p_begin != NULL )
            {
                memcpy( p_newdata, p_buffer->p_begin, p_buffer->i_length );
                free( p_buffer->p_memory );
            }
            p_buffer->p_memory = p_buffer->p_begin = p_newdata;
            p_buffer->i_size = i_newsize;
        }
    }
    /* Add the new data to the end of the current */
    memcpy( p_buffer->p_begin + p_buffer->i_length, p_data, i_len );
    p_buffer->i_length += i_len;
    return VLC_SUCCESS;
}
