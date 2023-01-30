}
static int Direct3DCompileShader(vout_display_t *vd, const char *shader_source, size_t source_length)
{
    vout_display_sys_t *sys = vd->sys;
    HRESULT (WINAPI * OurD3DXCompileShader)(
            LPCSTR pSrcData,
            UINT srcDataLen,
            const D3DXMACRO *pDefines,
            LPD3DXINCLUDE pInclude,
            LPCSTR pFunctionName,
            LPCSTR pProfile,
            DWORD Flags,
            LPD3DXBUFFER *ppShader,
            LPD3DXBUFFER *ppErrorMsgs,
            LPD3DXCONSTANTTABLE *ppConstantTable);
    OurD3DXCompileShader = (void*)GetProcAddress(sys->hd3d9x_dll, "D3DXCompileShader");
    if (!OurD3DXCompileShader) {
        msg_Warn(vd, "Cannot locate reference to D3DXCompileShader; pixel shading will be disabled");
        return VLC_EGENERIC;
    }
    LPD3DXBUFFER error_msgs = NULL;
    LPD3DXBUFFER compiled_shader = NULL;
    DWORD shader_flags = 0;
    HRESULT hr = OurD3DXCompileShader(shader_source, source_length, NULL, NULL,
                "main", "ps_3_0", shader_flags, &compiled_shader, &error_msgs, NULL);
    if (FAILED(hr)) {
        msg_Warn(vd, "D3DXCompileShader Error (hr=0x%lX)", hr);
        if (error_msgs)
            msg_Warn(vd, "HLSL Compilation Error: %s", (char*)ID3DXBuffer_GetBufferPointer(error_msgs));
        return VLC_EGENERIC;
    }
    hr = IDirect3DDevice9_CreatePixelShader(sys->d3ddev,
            ID3DXBuffer_GetBufferPointer(compiled_shader),
            &sys->d3dx_shader);
    if (FAILED(hr)) {
        msg_Warn(vd, "IDirect3DDevice9_CreatePixelShader error (hr=0x%lX)", hr);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
