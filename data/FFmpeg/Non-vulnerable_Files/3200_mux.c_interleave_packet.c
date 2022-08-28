static int interleave_packet(AVFormatContext *s, AVPacket *out, AVPacket *in, int flush)
{
    if (s->oformat->interleave_packet) {
        int ret = s->oformat->interleave_packet(s, out, in, flush);
        if (in)
            av_free_packet(in);
        return ret;
    } else
        return ff_interleave_packet_per_dts(s, out, in, flush);
}
