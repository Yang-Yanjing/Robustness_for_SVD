static void envelope(WaveformContext *s, AVFrame *out, int plane, int component)
{
    if (s->envelope == 0) {
        return;
    } else if (s->envelope == 1) {
        envelope_instant(s, out, plane, component);
    } else {
        envelope_peak(s, out, plane, component);
    }
}
