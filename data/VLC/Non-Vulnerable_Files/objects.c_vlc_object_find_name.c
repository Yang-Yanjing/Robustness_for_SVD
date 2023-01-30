 */
vlc_object_t *vlc_object_find_name( vlc_object_t *p_this, const char *psz_name )
{
    vlc_object_t *p_found;
    /* The object name is not thread-safe, provides no warranty that the
     * object is fully initialized and still active, and that its owner can
     * deal with asynchronous and external state changes. There may be multiple
     * objects with the same name, and the function may fail even if a matching
     * object exists. DO NOT USE THIS IN NEW CODE. */
#ifndef NDEBUG
    /* This was officially deprecated on August 19 2009. For the convenience of
     * wannabe code janitors, this is the list of names that remain used
     * and unfixed since then. */
    static const char bad[][11] = { "adjust", "clone", "colorthres",
        "erase", "extract", "gradient", "logo", "marq", "motionblur", "puzzle",
        "rotate", "sharpen", "transform", "v4l2", "wall" };
    static const char poor[][13] = { "invert", "magnify", "motiondetect",
        "psychedelic", "ripple", "wave" };
    if( bsearch( psz_name, bad, 15, 11, (void *)strcmp ) == NULL
     && bsearch( psz_name, poor, 6, 13, (void *)strcmp ) == NULL )
        return NULL;
    msg_Err( p_this, "looking for object \"%s\"... FIXME XXX", psz_name );
#endif
    libvlc_lock (p_this->p_libvlc);
    vlc_mutex_lock (&name_lock);
    p_found = FindName (vlc_internals (p_this), psz_name);
    vlc_mutex_unlock (&name_lock);
    libvlc_unlock (p_this->p_libvlc);
    return p_found;
}
