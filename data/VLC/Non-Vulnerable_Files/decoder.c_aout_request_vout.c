}
static vout_thread_t *aout_request_vout( void *p_private,
                                         vout_thread_t *p_vout, video_format_t *p_fmt, bool b_recyle )
{
    decoder_t *p_dec = p_private;
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    input_thread_t *p_input = p_owner->p_input;
    p_vout = input_resource_RequestVout( p_owner->p_resource, p_vout, p_fmt, 1,
                                         b_recyle );
    if( p_input != NULL )
        input_SendEventVout( p_input );
    return p_vout;
}
