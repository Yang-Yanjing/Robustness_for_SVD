static void encode_pulses(AACEncContext *s, Pulse *pulse)
{
    int i;
    put_bits(&s->pb, 1, !!pulse->num_pulse);
    if (!pulse->num_pulse)
        return;
    put_bits(&s->pb, 2, pulse->num_pulse - 1);
    put_bits(&s->pb, 6, pulse->start);
    for (i = 0; i < pulse->num_pulse; i++) {
        put_bits(&s->pb, 5, pulse->pos[i]);
        put_bits(&s->pb, 4, pulse->amp[i]);
    }
}
