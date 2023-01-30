}
void *module_Lookup( module_handle_t handle, const char *psz_function )
{
    return (void *)GetProcAddress( handle, (char *)psz_function );
}
