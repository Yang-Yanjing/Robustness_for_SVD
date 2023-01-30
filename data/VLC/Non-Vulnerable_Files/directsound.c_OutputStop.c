}
static void OutputStop( audio_output_t *aout )
{
    msg_Dbg( aout, "closing audio device" );
    Stop( &aout->sys->s );
}
