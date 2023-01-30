 *****************************************************************************/
static ULONG APIENTRY KaiCallback( PVOID p_cb_data,
                                   PVOID p_buffer,
                                   ULONG i_buf_size )
{
    audio_output_t *p_aout = (audio_output_t *)p_cb_data;
    int i_len;
    i_len = ReadBuffer( p_aout, p_buffer, i_buf_size );
    if(( ULONG )i_len < i_buf_size )
        memset(( uint8_t * )p_buffer + i_len, 0, i_buf_size - i_len );
    return i_buf_size;
}
