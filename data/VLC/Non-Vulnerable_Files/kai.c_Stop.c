 *****************************************************************************/
static void Stop ( audio_output_t *p_aout )
{
    aout_sys_t *p_sys = p_aout->sys;
    kaiClose( p_sys->hkai );
    kaiDone();
    DestroyBuffer( p_aout );
}
