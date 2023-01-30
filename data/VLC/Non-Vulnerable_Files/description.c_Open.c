 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    p_stream->pf_add  = Add;
    p_stream->pf_del  = Del;
    p_stream->pf_send = Send;
    p_sys = p_stream->p_sys = malloc(sizeof(sout_stream_sys_t));
    p_sys->data = var_InheritAddress(p_stream, "sout-description-data");
    if (p_sys->data == NULL)
    {
        msg_Err(p_stream, "Missing data: the description stream output is "
                "not meant to be used without special setup from the core");
        free(p_sys);
        return VLC_EGENERIC;
    }
    p_sys->i_stream_start = 0;
    return VLC_SUCCESS;
}
