/*** Initialization / deinitialization **/
static wchar_t *var_InheritWide(vlc_object_t *obj, const char *name)
{
    char *v8 = var_InheritString(obj, name);
    if (v8 == NULL)
        return NULL;
    wchar_t *v16 = ToWide(v8);
    free(v8);
    return v16;
}
