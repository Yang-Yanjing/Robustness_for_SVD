}
static void SplitterPrepare(vout_display_t *vd,
                            picture_t *picture,
                            subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    picture_Hold(picture);
    assert(!subpicture);
    if (video_splitter_Filter(sys->splitter, sys->picture, picture)) {
        for (int i = 0; i < sys->count; i++)
            sys->picture[i] = NULL;
        picture_Release(picture);
        return;
    }
    for (int i = 0; i < sys->count; i++) {
        if (vout_IsDisplayFiltered(sys->display[i]))
            sys->picture[i] = vout_FilterDisplay(sys->display[i], sys->picture[i]);
        if (sys->picture[i])
            vout_display_Prepare(sys->display[i], sys->picture[i], NULL);
    }
}
