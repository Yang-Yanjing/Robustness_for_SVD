 *****************************************************************************/
static int DecoderGetInputAttachments( decoder_t *p_dec,
                                       input_attachment_t ***ppp_attachment,
                                       int *pi_attachment )
{
    input_thread_t *p_input = p_dec->p_owner->p_input;
    if( unlikely(p_input == NULL) )
        return VLC_ENOOBJ;
    return input_Control( p_input, INPUT_GET_ATTACHMENTS,
                          ppp_attachment, pi_attachment );
}
