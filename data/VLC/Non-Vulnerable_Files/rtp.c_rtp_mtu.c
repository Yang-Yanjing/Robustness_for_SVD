 */
size_t rtp_mtu (const sout_stream_id_sys_t *id)
{
    return id->i_mtu - 12;
}
