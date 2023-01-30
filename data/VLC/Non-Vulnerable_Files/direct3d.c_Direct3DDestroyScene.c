 */
static void Direct3DDestroyScene(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    Direct3DDeleteRegions(sys->d3dregion_count, sys->d3dregion);
    LPDIRECT3DVERTEXBUFFER9 d3dvtc = sys->d3dvtc;
    if (d3dvtc)
        IDirect3DVertexBuffer9_Release(d3dvtc);
    LPDIRECT3DTEXTURE9 d3dtex = sys->d3dtex;
    if (d3dtex)
        IDirect3DTexture9_Release(d3dtex);
    sys->d3dvtc = NULL;
    sys->d3dtex = NULL;
    sys->d3dregion_count = 0;
    sys->d3dregion       = NULL;
    msg_Dbg(vd, "Direct3D scene released successfully");
}
