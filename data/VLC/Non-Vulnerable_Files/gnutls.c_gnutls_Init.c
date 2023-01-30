#if (GNUTLS_VERSION_NUMBER >= 0x030300)
static int gnutls_Init (vlc_object_t *obj)
{
    const char *version = gnutls_check_version ("3.3.0");
    if (version == NULL)
    {
        msg_Err (obj, "unsupported GnuTLS version");
        return -1;
    }
    msg_Dbg (obj, "using GnuTLS version %s", version);
    return 0;
}
 */
static int gnutls_Init (vlc_object_t *obj)
{
    const char *version = gnutls_check_version ("3.0.20");
    if (version == NULL)
    {
        msg_Err (obj, "unsupported GnuTLS version");
        return -1;
    }
    msg_Dbg (obj, "using GnuTLS version %s", version);
    if (gnutls_check_version ("3.3.0") == NULL)
    {
         int val;
         vlc_mutex_lock (&gnutls_mutex);
         val = gnutls_global_init ();
         vlc_mutex_unlock (&gnutls_mutex);
         if (val)
         {
             msg_Err (obj, "cannot initialize GnuTLS");
             return -1;
         }
    }
    return 0;
}
