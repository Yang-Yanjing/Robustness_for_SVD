 *****************************************************************************/
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    VLC_UNUSED(p_stream);
    id->b_used = false;
    return VLC_SUCCESS;
}
