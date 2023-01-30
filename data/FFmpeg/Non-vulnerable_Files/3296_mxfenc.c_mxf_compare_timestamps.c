static int mxf_compare_timestamps(AVFormatContext *s, AVPacket *next, AVPacket *pkt)
{
    MXFStreamContext *sc  = s->streams[pkt ->stream_index]->priv_data;
    MXFStreamContext *sc2 = s->streams[next->stream_index]->priv_data;
    return next->dts > pkt->dts ||
        (next->dts == pkt->dts && sc->order < sc2->order);
}
