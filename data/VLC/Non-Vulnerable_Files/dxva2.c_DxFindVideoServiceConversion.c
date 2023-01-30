 */
static int DxFindVideoServiceConversion(vlc_va_t *va, GUID *input, D3DFORMAT *output)
{
    vlc_va_sys_t *sys = va->sys;
    /* Retreive supported modes from the decoder service */
    UINT input_count = 0;
    GUID *input_list = NULL;
    if (FAILED(IDirectXVideoDecoderService_GetDecoderDeviceGuids(sys->vs,
                                                                 &input_count,
                                                                 &input_list))) {
        msg_Err(va, "IDirectXVideoDecoderService_GetDecoderDeviceGuids failed");
        return VLC_EGENERIC;
    }
    for (unsigned i = 0; i < input_count; i++) {
        const GUID *g = &input_list[i];
        const dxva2_mode_t *mode = Dxva2FindMode(g);
        if (mode) {
            msg_Dbg(va, "- '%s' is supported by hardware", mode->name);
        } else {
            msg_Warn(va, "- Unknown GUID = " GUID_FMT, GUID_PRINT( *g ) );
        }
    }
    /* Try all supported mode by our priority */
    for (unsigned i = 0; dxva2_modes[i].name; i++) {
        const dxva2_mode_t *mode = &dxva2_modes[i];
        if (!mode->codec || mode->codec != sys->codec_id)
            continue;
        /* */
        bool is_supported = false;
        for (const GUID *g = &input_list[0]; !is_supported && g < &input_list[input_count]; g++) {
            is_supported = IsEqualGUID(mode->guid, g);
        }
        if (!is_supported)
            continue;
        /* */
        msg_Dbg(va, "Trying to use '%s' as input", mode->name);
        UINT      output_count = 0;
        D3DFORMAT *output_list = NULL;
        if (FAILED(IDirectXVideoDecoderService_GetDecoderRenderTargets(sys->vs, mode->guid,
                                                                       &output_count,
                                                                       &output_list))) {
            msg_Err(va, "IDirectXVideoDecoderService_GetDecoderRenderTargets failed");
            continue;
        }
        for (unsigned j = 0; j < output_count; j++) {
            const D3DFORMAT f = output_list[j];
            const d3d_format_t *format = D3dFindFormat(f);
            if (format) {
                msg_Dbg(va, "%s is supported for output", format->name);
            } else {
                msg_Dbg(va, "%d is supported for output (%4.4s)", f, (const char*)&f);
            }
        }
        /* */
        for (unsigned j = 0; d3d_formats[j].name; j++) {
            const d3d_format_t *format = &d3d_formats[j];
            /* */
            bool is_supported = false;
            for (unsigned k = 0; !is_supported && k < output_count; k++) {
                is_supported = format->format == output_list[k];
            }
            if (!is_supported)
                continue;
            /* We have our solution */
            msg_Dbg(va, "Using '%s' to decode to '%s'", mode->name, format->name);
            *input  = *mode->guid;
            *output = format->format;
            CoTaskMemFree(output_list);
            CoTaskMemFree(input_list);
            return VLC_SUCCESS;
        }
        CoTaskMemFree(output_list);
    }
    CoTaskMemFree(input_list);
    return VLC_EGENERIC;
}
