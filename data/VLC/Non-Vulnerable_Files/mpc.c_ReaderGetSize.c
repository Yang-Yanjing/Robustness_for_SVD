#ifndef HAVE_MPC_MPCDEC_H
static mpc_int32_t ReaderGetSize( void *p_private )
{
    demux_t *p_demux = (demux_t*)p_private;
#else
static mpc_int32_t ReaderGetSize( mpc_reader *p_private )
{
    demux_t *p_demux = (demux_t*)p_private->data;
#endif
    return stream_Size( p_demux->s );
}
#ifndef HAVE_MPC_MPCDEC_H
static mpc_bool_t ReaderCanSeek( void *p_private )
{
    demux_t *p_demux = (demux_t*)p_private;
#else
static mpc_bool_t ReaderCanSeek( mpc_reader *p_private )
{
    demux_t *p_demux = (demux_t*)p_private->data;
#endif
    bool b_canseek;
    stream_Control( p_demux->s, STREAM_CAN_SEEK, &b_canseek );
    return b_canseek;
}
