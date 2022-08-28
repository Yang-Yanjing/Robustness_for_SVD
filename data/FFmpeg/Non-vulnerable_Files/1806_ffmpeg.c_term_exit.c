void term_exit(void)
{
    av_log(NULL, AV_LOG_QUIET, "%s", "");
    term_exit_sigsafe();
}
