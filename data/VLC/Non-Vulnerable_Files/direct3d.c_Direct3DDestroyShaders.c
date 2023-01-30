}
static void Direct3DDestroyShaders(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->d3dx_shader)
        IDirect3DPixelShader9_Release(sys->d3dx_shader);
    sys->d3dx_shader = NULL;
}
