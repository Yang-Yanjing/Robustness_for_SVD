static void write_time(AVIOContext *out, int64_t time)
{
    int seconds = time / AV_TIME_BASE;
    int fractions = time % AV_TIME_BASE;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;
    avio_printf(out, "PT");
    if (hours)
        avio_printf(out, "%dH", hours);
    if (hours || minutes)
        avio_printf(out, "%dM", minutes);
    avio_printf(out, "%d.%dS", seconds, fractions / (AV_TIME_BASE / 10));
}
