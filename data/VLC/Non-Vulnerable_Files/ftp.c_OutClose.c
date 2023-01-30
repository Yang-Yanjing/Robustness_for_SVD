#ifdef ENABLE_SOUT
static void OutClose( vlc_object_t *p_this )
{
    Close( p_this, GET_OUT_SYS(p_this));
}
