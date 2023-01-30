/* */
static void Close(vlc_object_t *object)
{
    access_t     *access = (access_t *)object;
    access_sys_t *sys = access->p_sys;
    vlc_input_attachment_Delete(sys->a);
    free(sys);
}
