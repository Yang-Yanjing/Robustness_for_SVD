}
static void SplitterClose(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    /* */
    video_splitter_t *splitter = sys->splitter;
    free(splitter->p_owner);
    video_splitter_Delete(splitter);
    if (sys->pool)
        picture_pool_Delete(sys->pool);
    /* */
    for (int i = 0; i < sys->count; i++)
        vout_DeleteDisplay(sys->display[i], NULL);
    TAB_CLEAN(sys->count, sys->display);
    free(sys->picture);
    free(sys);
}
