}
static int Direct3DRenderRegion(vout_display_t *vd,
                                d3d_region_t *region,
                                bool use_pixel_shader)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3DDEVICE9 d3ddev = vd->sys->d3ddev;
    LPDIRECT3DVERTEXBUFFER9 d3dvtc = sys->d3dvtc;
    LPDIRECT3DTEXTURE9      d3dtex = region->texture;
    HRESULT hr;
    /* Import vertices */
    void *vertex;
    hr = IDirect3DVertexBuffer9_Lock(d3dvtc, 0, 0, &vertex, D3DLOCK_DISCARD);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return -1;
    }
    memcpy(vertex, region->vertex, sizeof(region->vertex));
    hr = IDirect3DVertexBuffer9_Unlock(d3dvtc);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return -1;
    }
    // Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.
    hr = IDirect3DDevice9_SetTexture(d3ddev, 0, (LPDIRECT3DBASETEXTURE9)d3dtex);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return -1;
    }
    if (sys->d3dx_shader) {
        if (use_pixel_shader)
        {
            hr = IDirect3DDevice9_SetPixelShader(d3ddev, sys->d3dx_shader);
            float shader_data[4] = { region->width, region->height, 0, 0 };
            hr = IDirect3DDevice9_SetPixelShaderConstantF(d3ddev, 0, shader_data, 1);
            if (FAILED(hr)) {
                msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
                return -1;
            }
        }
        else /* Disable any existing pixel shader. */
            hr = IDirect3DDevice9_SetPixelShader(d3ddev, NULL);
        if (FAILED(hr)) {
            msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
            return -1;
        }
    }
    // Render the vertex buffer contents
    hr = IDirect3DDevice9_SetStreamSource(d3ddev, 0, d3dvtc, 0, sizeof(CUSTOMVERTEX));
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return -1;
    }
    // we use FVF instead of vertex shader
    hr = IDirect3DDevice9_SetFVF(d3ddev, D3DFVF_CUSTOMVERTEX);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return -1;
    }
    // draw rectangle
    hr = IDirect3DDevice9_DrawPrimitive(d3ddev, D3DPT_TRIANGLEFAN, 0, 2);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return -1;
    }
    return 0;
}
