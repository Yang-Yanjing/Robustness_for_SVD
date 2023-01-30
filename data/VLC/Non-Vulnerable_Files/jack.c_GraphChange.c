 *****************************************************************************/
static int GraphChange( void *p_arg )
{
  audio_output_t *p_aout = (audio_output_t*) p_arg;
  struct aout_sys_t *p_sys = p_aout->sys;
  unsigned int i;
  jack_latency_range_t port_latency;
  p_sys->latency = 0;
  for( i = 0; i < p_sys->i_channels; ++i )
  {
    jack_port_get_latency_range( p_sys->p_jack_ports[i], JackPlaybackLatency,
                                 &port_latency );
    p_sys->latency = __MAX( p_sys->latency, port_latency.max );
  }
  msg_Dbg(p_aout, "JACK graph reordered. Our maximum latency=%d.",
          p_sys->latency);
  return 0;
}
