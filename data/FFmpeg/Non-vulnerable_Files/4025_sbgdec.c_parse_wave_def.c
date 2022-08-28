static int parse_wave_def(struct sbg_parser *p, int wavenum)
{
    snprintf(p->err_msg, sizeof(p->err_msg),
             "waveform definitions not yet implemented");
    return AVERROR_PATCHWELCOME;
}
