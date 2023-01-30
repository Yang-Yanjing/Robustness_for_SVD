}
int lua_ExecuteFunction( extensions_manager_t *p_mgr, extension_t *p_ext,
                            const char *psz_function, ... )
{
    va_list args;
    va_start( args, psz_function );
    int i_ret = lua_ExecuteFunctionVa( p_mgr, p_ext, psz_function, args );
    va_end( args );
    return i_ret;
}
