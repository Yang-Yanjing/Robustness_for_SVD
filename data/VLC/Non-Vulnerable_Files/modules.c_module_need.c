#undef module_need
module_t *module_need(vlc_object_t *obj, const char *cap, const char *name,
                      bool strict)
{
    return vlc_module_load(obj, cap, name, strict, generic_start, obj);
}
