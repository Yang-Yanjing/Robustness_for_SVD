 *****************************************************************************/
static void DeleteSong(audioscrobbler_song_t* p_song)
{
    FREENULL(p_song->psz_a);
    FREENULL(p_song->psz_b);
    FREENULL(p_song->psz_t);
    FREENULL(p_song->psz_m);
    FREENULL(p_song->psz_n);
}
