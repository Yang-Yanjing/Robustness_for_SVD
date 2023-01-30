}
static int AddStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    if( p_mux->i_nb_inputs > 1 )
    {
        msg_Dbg( p_mux, "only 1 input allowed" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_mux, "adding input" );
    if( p_input->p_fmt->i_codec != VLC_CODEC_MJPG )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
