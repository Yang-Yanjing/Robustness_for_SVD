static void envelope16(WaveformContext *s, AVFrame *out, int plane, int component)
{
    if (s->envelope == 0) {
        return;
    } else if (s->envelope == 1) {
        envelope_instant16(s, out, plane, component);
    } else {
        envelope_peak16(s, out, plane, component);
    }
}
