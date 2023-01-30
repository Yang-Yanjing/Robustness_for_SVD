}
static void Direct3DDeleteRegions(int count, d3d_region_t *region)
{
    for (int i = 0; i < count; i++) {
        if (region[i].texture)
            IDirect3DTexture9_Release(region[i].texture);
    }
    free(region);
}
