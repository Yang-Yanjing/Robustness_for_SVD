}
static void OutputFlush( audio_output_t *aout, bool drain )
{
    aout_stream_sys_t *sys = &aout->sys->s;
    Flush( sys, drain );
}
