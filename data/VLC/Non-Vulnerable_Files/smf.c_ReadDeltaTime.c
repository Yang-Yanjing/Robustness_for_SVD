 */
static int ReadDeltaTime (stream_t *s, mtrk_t *track)
{
    int32_t delta_time;
    assert (stream_Tell (s) == track->start + track->offset);
    if (track->offset >= track->length)
    {
        /* This track is done */
        track->next = UINT64_MAX;
        return 0;
    }
    delta_time = ReadVarInt (s);
    if (delta_time < 0)
        return -1;
    track->next += delta_time;
    track->offset = stream_Tell (s) - track->start;
    return 0;
}
