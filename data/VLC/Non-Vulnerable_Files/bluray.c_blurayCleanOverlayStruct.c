 *****************************************************************************/
static void blurayCleanOverlayStruct(bluray_overlay_t *p_overlay)
{
    if (!atomic_flag_test_and_set(&p_overlay->released_once))
        return;
    /*
     * This will be called when destroying the picture.
     * Don't delete it again from here!
     */
    vlc_mutex_destroy(&p_overlay->lock);
    subpicture_region_ChainDelete(p_overlay->p_regions);
    free(p_overlay);
}
