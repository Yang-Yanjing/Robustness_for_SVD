static int64_t get_delay(ASyncContext *s)
{
    return avresample_available(s->avr) + avresample_get_delay(s->avr);
}
