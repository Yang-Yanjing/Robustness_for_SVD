/* */
static int Open(vlc_object_t *object)
{
    access_t     *access = (access_t *)object;
    access_sys_t *sys;
    input_thread_t *input = access_GetParentInput(access);
    if (!input)
        return VLC_EGENERIC;
    input_attachment_t *a;
    if (input_Control(input, INPUT_GET_ATTACHMENT, &a, access->psz_location))
        a = NULL;
    vlc_object_release(input);
    if (!a) {
        msg_Err(access, "Failed to find the attachment '%s'",
                access->psz_location);
        return VLC_EGENERIC;
    }
    /* */
    access->p_sys = sys = malloc(sizeof(*sys));
    if (!sys) {
        vlc_input_attachment_Delete(a);
        return VLC_ENOMEM;
    }
    sys->a = a;
    /* */
    access_InitFields(access);
    access->pf_read    = Read;
    access->pf_block   = NULL;
    access->pf_control = Control;
    access->pf_seek    = Seek;
    return VLC_SUCCESS;
}
