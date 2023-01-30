 */
void vlc_vaLog (vlc_object_t *obj, int type, const char *module,
                const char *format, va_list args)
{
    if (obj != NULL && obj->i_flags & OBJECT_FLAGS_QUIET)
        return;
    /* Get basename from the module filename */
    char *p = strrchr(module, '/');
    if (p != NULL)
        module = p;
    p = strchr(module, '.');
    size_t modlen = (p != NULL) ? (p - module) : 1;
    char modulebuf[modlen + 1];
    if (p != NULL)
    {
        memcpy(modulebuf, module, modlen);
        modulebuf[modlen] = '\0';
        module = modulebuf;
    }
    /* Fill message information fields */
    vlc_log_t msg;
    msg.i_object_id = (uintptr_t)obj;
    msg.psz_object_type = (obj != NULL) ? obj->psz_object_type : "generic";
    msg.psz_module = module;
    msg.psz_header = NULL;
    for (vlc_object_t *o = obj; o != NULL; o = o->p_parent)
        if (o->psz_header != NULL)
        {
            msg.psz_header = o->psz_header;
            break;
        }
    /* Pass message to the callback */
    libvlc_priv_t *priv = obj ? libvlc_priv (obj->p_libvlc) : NULL;
#ifdef _WIN32
    va_list ap;
    va_copy (ap, args);
    Win32DebugOutputMsg (priv ? &priv->log.verbose : NULL, type, &msg, format, ap);
    va_end (ap);
#endif
    if (priv) {
        int canc = vlc_savecancel ();
        vlc_rwlock_rdlock (&priv->log.lock);
        priv->log.cb (priv->log.opaque, type, &msg, format, args);
        vlc_rwlock_unlock (&priv->log.lock);
        vlc_restorecancel (canc);
    }
}
