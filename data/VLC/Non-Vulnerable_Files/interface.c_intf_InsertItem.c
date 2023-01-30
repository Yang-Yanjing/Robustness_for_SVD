 */
void intf_InsertItem(libvlc_int_t *libvlc, const char *mrl, unsigned optc,
                     const char *const *optv, unsigned flags)
{
    playlist_AddExt(intf_GetPlaylist(libvlc), mrl, NULL, PLAYLIST_INSERT,
                    0, -1, optc, optv, flags, true, pl_Unlocked);
}
