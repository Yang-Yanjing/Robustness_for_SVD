static av_cold void init_vlcs(FourXContext *f)
{
    static VLC_TYPE table[2][4][32][2];
    int i, j;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            block_type_vlc[i][j].table           = table[i][j];
            block_type_vlc[i][j].table_allocated = 32;
            init_vlc(&block_type_vlc[i][j], BLOCK_TYPE_VLC_BITS, 7,
                     &block_type_tab[i][j][0][1], 2, 1,
                     &block_type_tab[i][j][0][0], 2, 1,
                     INIT_VLC_USE_NEW_STATIC);
        }
    }
}
