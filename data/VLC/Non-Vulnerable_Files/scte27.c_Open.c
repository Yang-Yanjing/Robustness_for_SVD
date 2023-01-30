}
static int Open(vlc_object_t *object)
{
    decoder_t *dec = (decoder_t *)object;
    if (dec->fmt_in.i_codec != VLC_CODEC_SCTE_27)
        return VLC_EGENERIC;
    decoder_sys_t *sys = dec->p_sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->segment_id = -1;
    sys->segment_size = 0;
    sys->segment_buffer = NULL;
    dec->pf_decode_sub = Decode;
    es_format_Init(&dec->fmt_out, SPU_ES, VLC_CODEC_SPU);
    dec->fmt_out.video.i_chroma = VLC_CODEC_YUVP;
    return VLC_SUCCESS;
}
