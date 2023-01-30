static void envelope(VectorscopeContext *s, AVFrame *out)
{
    if (!s->envelope) {
        return;
    } else if (s->envelope == 1) {
        envelope_instant(s, out);
    } else {
        envelope_peak(s, out);
    }
}
