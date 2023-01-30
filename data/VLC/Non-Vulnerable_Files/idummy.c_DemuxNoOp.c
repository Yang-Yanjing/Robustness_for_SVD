static int DemuxControl( demux_t *, int, va_list );
static int DemuxNoOp( demux_t *demux )
{
    (void) demux;
    return 0;
}
