 */
int libvlc_ArtRequest(libvlc_int_t *libvlc, input_item_t *item,
                      input_item_meta_request_option_t i_options)
{
    libvlc_priv_t *priv = libvlc_priv(libvlc);
    if (unlikely(priv->parser == NULL))
        return VLC_ENOMEM;
    playlist_preparser_fetcher_Push(priv->parser, item, i_options);
    return VLC_SUCCESS;
}
