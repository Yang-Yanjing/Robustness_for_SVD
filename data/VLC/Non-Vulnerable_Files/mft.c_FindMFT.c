}
static int FindMFT(decoder_t *p_dec)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    MFHandle *mf = &p_sys->mf_handle;
    HRESULT hr;
    /* Try to create a MFT using MFTEnumEx. */
    GUID category;
    if (p_dec->fmt_in.i_cat == VIDEO_ES)
    {
        category = MFT_CATEGORY_VIDEO_DECODER;
        p_sys->major_type = &MFMediaType_Video;
        p_sys->subtype = FormatToGUID(video_format_table, p_dec->fmt_in.i_codec);
    }
    else
    {
        category = MFT_CATEGORY_AUDIO_DECODER;
        p_sys->major_type = &MFMediaType_Audio;
        p_sys->subtype = FormatToGUID(audio_format_table, p_dec->fmt_in.i_codec);
    }
    if (!p_sys->subtype)
        return VLC_EGENERIC;
    UINT32 flags = MFT_ENUM_FLAG_SORTANDFILTER | MFT_ENUM_FLAG_LOCALMFT
                 | MFT_ENUM_FLAG_SYNCMFT | MFT_ENUM_FLAG_ASYNCMFT
                 | MFT_ENUM_FLAG_HARDWARE | MFT_ENUM_FLAG_TRANSCODE_ONLY;
    MFT_REGISTER_TYPE_INFO input_type = { *p_sys->major_type, *p_sys->subtype };
    IMFActivate **activate_objects = NULL;
    UINT32 activate_objects_count = 0;
    hr = mf->MFTEnumEx(category, flags, &input_type, NULL, &activate_objects, &activate_objects_count);
    if (FAILED(hr))
        return VLC_EGENERIC;
    msg_Dbg(p_dec, "Found %d available MFT module(s)", activate_objects_count);
    if (activate_objects_count == 0)
        return VLC_EGENERIC;
    for (UINT32 i = 0; i < activate_objects_count; ++i)
    {
        hr = IMFActivate_ActivateObject(activate_objects[i], &IID_IMFTransform, (void**)&p_sys->mft);
        IMFActivate_Release(activate_objects[i]);
        if (FAILED(hr))
            continue;
        if (InitializeMFT(p_dec) == VLC_SUCCESS)
        {
            CoTaskMemFree(activate_objects);
            return VLC_SUCCESS;
        }
    }
    CoTaskMemFree(activate_objects);
    return VLC_EGENERIC;
}
