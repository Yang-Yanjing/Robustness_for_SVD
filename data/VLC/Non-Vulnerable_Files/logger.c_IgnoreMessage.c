}
static bool IgnoreMessage( intf_thread_t *p_intf, int type )
{
    /* TODO: cache value... */
    int verbosity = var_InheritInteger( p_intf, "log-verbose" );
    if (verbosity == -1)
        verbosity = var_InheritInteger( p_intf, "verbose" );
    return verbosity < 0 || verbosity < (type - VLC_MSG_ERR);
}
