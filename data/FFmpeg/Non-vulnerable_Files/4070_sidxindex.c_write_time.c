static void write_time(FILE *out, int64_t time, int decimals, enum AVRounding round)
{
    int seconds = time / AV_TIME_BASE;
    int fractions = time % AV_TIME_BASE;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    fractions = av_rescale_rnd(fractions, pow(10, decimals), AV_TIME_BASE, round);
    seconds %= 60;
    minutes %= 60;
    fprintf(out, "PT");
    if (hours)
        fprintf(out, "%dH", hours);
    if (hours || minutes)
        fprintf(out, "%dM", minutes);
    fprintf(out, "%d.%0*dS", seconds, decimals, fractions);
}
