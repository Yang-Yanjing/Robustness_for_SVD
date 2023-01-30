 */
int decoder_GetInputAttachments( decoder_t *p_dec,
                                 input_attachment_t ***ppp_attachment,
                                 int *pi_attachment )
{
    if( !p_dec->pf_get_attachments )
        return VLC_EGENERIC;
    return p_dec->pf_get_attachments( p_dec, ppp_attachment, pi_attachment );
}
