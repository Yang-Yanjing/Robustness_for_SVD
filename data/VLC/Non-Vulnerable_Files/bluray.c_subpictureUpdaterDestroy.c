static void blurayCleanOverlayStruct(bluray_overlay_t *);
static void subpictureUpdaterDestroy(subpicture_t *p_subpic)
{
    blurayCleanOverlayStruct(p_subpic->updater.p_sys->p_overlay);
    free(p_subpic->updater.p_sys);
}
