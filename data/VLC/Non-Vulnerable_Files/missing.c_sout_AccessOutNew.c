#undef sout_AccessOutNew
sout_access_out_t *sout_AccessOutNew (vlc_object_t *obj,
                                      const char *access, const char *name)
{
    VLC_UNUSED (access); VLC_UNUSED (name);
    msg_Err (obj, "Output support not compiled-in!");
    return NULL;
}
