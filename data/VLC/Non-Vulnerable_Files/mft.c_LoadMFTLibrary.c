}
static int LoadMFTLibrary(MFHandle *mf)
{
    mf->mfplat_dll = LoadLibrary(TEXT("mfplat.dll"));
    if (!mf->mfplat_dll)
        return VLC_EGENERIC;
    mf->MFTEnumEx = (void*)GetProcAddress(mf->mfplat_dll, "MFTEnumEx");
    mf->MFCreateSample = (void*)GetProcAddress(mf->mfplat_dll, "MFCreateSample");
    mf->MFCreateMemoryBuffer = (void*)GetProcAddress(mf->mfplat_dll, "MFCreateMemoryBuffer");
    mf->MFCreateAlignedMemoryBuffer = (void*)GetProcAddress(mf->mfplat_dll, "MFCreateAlignedMemoryBuffer");
    if (!mf->MFTEnumEx || !mf->MFCreateSample || !mf->MFCreateMemoryBuffer || !mf->MFCreateAlignedMemoryBuffer)
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
