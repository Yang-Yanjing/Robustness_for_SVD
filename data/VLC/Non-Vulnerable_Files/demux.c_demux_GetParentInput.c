 *****************************************************************************/
input_thread_t * demux_GetParentInput( demux_t *p_demux )
{
    return p_demux->p_input ? vlc_object_hold((vlc_object_t*)p_demux->p_input) : NULL;
}
