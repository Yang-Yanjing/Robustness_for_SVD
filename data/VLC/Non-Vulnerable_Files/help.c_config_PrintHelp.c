 */
bool config_PrintHelp (vlc_object_t *obj)
{
    char *str;
    /* Check for short help option */
    if (var_InheritBool (obj, "help"))
    {
        Help (obj, "help");
        return true;
    }
    /* Check for version option */
    if (var_InheritBool (obj, "version"))
    {
        Version();
        return true;
    }
    /* Check for help on modules */
    str = var_InheritString (obj, "module");
    if (str != NULL)
    {
        Help (obj, str);
        free (str);
        return true;
    }
    /* Check for full help option */
    if (var_InheritBool (obj, "full-help"))
    {
        var_Create (obj, "advanced", VLC_VAR_BOOL);
        var_SetBool (obj, "advanced", true);
        var_Create (obj, "help-verbose", VLC_VAR_BOOL);
        var_SetBool (obj, "help-verbose", true);
        Help (obj, "full-help");
        return true;
    }
    /* Check for long help option */
    if (var_InheritBool (obj, "longhelp"))
    {
        Help (obj, "longhelp");
        return true;
    }
    /* Check for module list option */
    if (var_InheritBool (obj, "list"))
    {
        ListModules (obj, false );
        return true;
    }
    if (var_InheritBool (obj, "list-verbose"))
    {
        ListModules (obj, true);
        return true;
    }
    return false;
}
