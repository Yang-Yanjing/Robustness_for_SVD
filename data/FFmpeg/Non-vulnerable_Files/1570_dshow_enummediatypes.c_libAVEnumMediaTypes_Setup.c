static int
libAVEnumMediaTypes_Setup(libAVEnumMediaTypes *this, const AM_MEDIA_TYPE *type)
{
    IEnumMediaTypesVtbl *vtbl = this->vtbl;
    SETVTBL(vtbl, libAVEnumMediaTypes, QueryInterface);
    SETVTBL(vtbl, libAVEnumMediaTypes, AddRef);
    SETVTBL(vtbl, libAVEnumMediaTypes, Release);
    SETVTBL(vtbl, libAVEnumMediaTypes, Next);
    SETVTBL(vtbl, libAVEnumMediaTypes, Skip);
    SETVTBL(vtbl, libAVEnumMediaTypes, Reset);
    SETVTBL(vtbl, libAVEnumMediaTypes, Clone);
    if (!type) {
        this->type.majortype = GUID_NULL;
    } else {
        ff_copy_dshow_media_type(&this->type, type);
    }
    return 1;
}
