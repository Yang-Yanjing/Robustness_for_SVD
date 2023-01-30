/* */
static void DecoderUnsupportedCodec( decoder_t *p_dec, vlc_fourcc_t codec )
{
    if (codec != VLC_FOURCC('u','n','d','f')) {
        const char *desc = vlc_fourcc_GetDescription(p_dec->fmt_in.i_cat, codec);
        if (!desc || !*desc)
            desc = N_("No description for this codec");
        msg_Err( p_dec, "Codec `%4.4s' (%s) is not supported.", (char*)&codec, desc );
        dialog_Fatal( p_dec, _("Codec not supported"),
                _("VLC could not decode the format \"%4.4s\" (%s)"),
                (char*)&codec, desc );
    } else {
        msg_Err( p_dec, "could not identify codec" );
        dialog_Fatal( p_dec, _("Unidentified codec"),
            _("VLC could not identify the audio or video codec" ) );
    }
}
