 */
static void Direct3DDestroyResources(vout_display_t *vd)
{
    Direct3DDestroyScene(vd);
    Direct3DDestroyPool(vd);
    Direct3DDestroyShaders(vd);
}
