}
static int CreateDefaultHeader( decoder_t *p_dec )
{
    ogg_packet oggpacket;
    SpeexHeader *p_header = malloc( sizeof(SpeexHeader) );
    if( !p_header )
        return VLC_ENOMEM;
    const int rate = p_dec->fmt_in.audio.i_rate;
    const unsigned i_mode = (rate / 8000) >> 1;
    const SpeexMode *mode;
    int ret = VLC_SUCCESS;
    oggpacket.packet = NULL;
    switch( rate )
    {
        case 8000:
        case 16000:
        case 32000:
            mode = speex_lib_get_mode( i_mode );
            break;
        default:
            msg_Err( p_dec, "Unexpected rate %d", rate );
            ret = VLC_EGENERIC;
            goto cleanup;
    }
    speex_init_header( p_header, rate, p_dec->fmt_in.audio.i_channels, mode );
    p_header->frames_per_packet = 160 << i_mode;
    oggpacket.packet = (unsigned char *) speex_header_to_packet( p_header,
            (int *) &oggpacket.bytes );
    if( !oggpacket.packet )
    {
        ret = VLC_ENOMEM;
        goto cleanup;
    }
    oggpacket.b_o_s = 1;
    oggpacket.e_o_s = 0;
    oggpacket.granulepos = -1;
    oggpacket.packetno = 0;
    ret = ProcessInitialHeader( p_dec, &oggpacket );
    if( ret != VLC_SUCCESS )
    {
        msg_Err( p_dec, "default Speex header is corrupted" );
    }
cleanup:
    free( oggpacket.packet );
    free( p_header );
    return ret;
}
