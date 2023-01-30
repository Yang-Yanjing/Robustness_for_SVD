 *****************************************************************************/
static void DemuxForceStill( demux_t *p_demux )
{
    static const uint8_t buffer[] = {
        0x00, 0x00, 0x01, 0xe0, 0x00, 0x07,
        0x80, 0x00, 0x00,
        0x00, 0x00, 0x01, 0xB7,
    };
    DemuxBlock( p_demux, buffer, sizeof(buffer) );
    bool b_empty;
    es_out_Control( p_demux->out, ES_OUT_GET_EMPTY, &b_empty );
}
