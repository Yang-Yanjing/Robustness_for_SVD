static int mxf_interleave(AVFormatContext *s, AVPacket *out, AVPacket *pkt, int flush)
{
    return ff_audio_rechunk_interleave(s, out, pkt, flush,
                               mxf_interleave_get_packet, mxf_compare_timestamps);
}
