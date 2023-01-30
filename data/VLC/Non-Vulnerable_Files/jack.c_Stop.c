 *****************************************************************************/
static void Stop( audio_output_t *p_aout )
{
    int i_error;
    struct aout_sys_t *p_sys = p_aout->sys;
    i_error = jack_deactivate( p_sys->p_jack_client );
    if( i_error )
    {
        msg_Err( p_aout, "jack_deactivate failed (error %d)", i_error );
    }
    i_error = jack_client_close( p_sys->p_jack_client );
    if( i_error )
    {
        msg_Err( p_aout, "jack_client_close failed (error %d)", i_error );
    }
    free( p_sys->p_jack_ports );
    free( p_sys->p_jack_buffers );
    jack_ringbuffer_free( p_sys->p_jack_ringbuffer );
}
