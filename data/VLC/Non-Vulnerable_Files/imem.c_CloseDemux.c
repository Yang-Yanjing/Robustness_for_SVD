 */
static void CloseDemux(vlc_object_t *object)
{
    demux_t *demux = (demux_t *)object;
    CloseCommon((imem_sys_t*)demux->p_sys);
}
