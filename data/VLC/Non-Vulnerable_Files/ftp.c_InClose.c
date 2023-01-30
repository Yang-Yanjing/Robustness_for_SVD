}
static void InClose( vlc_object_t *p_this )
{
    Close( p_this, ((access_t *)p_this)->p_sys);
}
