 *****************************************************************************/
static int Open(vlc_object_t *p_this)
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    if (p_dec->fmt_in.i_codec != VLC_CODEC_HEVC)
        return VLC_EGENERIC;
    p_dec->p_sys = calloc(1, sizeof(decoder_sys_t));
    if (!p_dec->p_sys)
        return VLC_ENOMEM;
    packetizer_Init(&p_dec->p_sys->packetizer,
                    p_hevc_startcode, sizeof(p_hevc_startcode),
                    p_hevc_startcode, 1, 5,
                    PacketizeReset, PacketizeParse, PacketizeValidate, p_dec);
    /* Copy properties */
    es_format_Copy(&p_dec->fmt_out, &p_dec->fmt_in);
    /* Set callback */
    p_dec->pf_packetize = Packetize;
    return VLC_SUCCESS;
}
