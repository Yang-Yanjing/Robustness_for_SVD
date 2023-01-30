                                const char *psz_access, const char *psz_path );
static void AppendAttachment( int *pi_attachment, input_attachment_t ***ppp_attachment, demux_t ***ppp_attachment_demux,
                              int i_new, input_attachment_t **pp_new, demux_t *p_demux );
enum {
    SUB_NOFLAG = 0x00,
    SUB_FORCED = 0x01,
    SUB_CANFAIL = 0x02,
};
}
static void AppendAttachment( int *pi_attachment, input_attachment_t ***ppp_attachment, demux_t ***ppp_attachment_demux,
                              int i_new, input_attachment_t **pp_new, demux_t *p_demux )
{
    int i_attachment = *pi_attachment;
    input_attachment_t **attachment = *ppp_attachment;
    demux_t **attachment_demux = *ppp_attachment_demux;
    int i;
    attachment = xrealloc( attachment,
                    sizeof(*attachment) * ( i_attachment + i_new ) );
    attachment_demux = xrealloc( attachment_demux,
                    sizeof(*attachment_demux) * ( i_attachment + i_new ) );
    for( i = 0; i < i_new; i++ )
    {
        attachment[i_attachment] = pp_new[i];
        attachment_demux[i_attachment++] = p_demux;
    }
    free( pp_new );
    /* */
    *pi_attachment = i_attachment;
    *ppp_attachment = attachment;
    *ppp_attachment_demux = attachment_demux;
}
