 */
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    D2D_DestroyRenderTarget(vd);
    if (vd->sys->pool)
        picture_pool_Delete(vd->sys->pool);
    CommonClean(vd);
    free(vd->sys);
}
