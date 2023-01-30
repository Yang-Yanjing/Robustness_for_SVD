}
static void SplitterDisplay(vout_display_t *vd,
                            picture_t *picture,
                            subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    assert(!subpicture);
    for (int i = 0; i < sys->count; i++) {
        if (sys->picture[i])
            vout_display_Display(sys->display[i], sys->picture[i], NULL);
    }
    picture_Release(picture);
}
