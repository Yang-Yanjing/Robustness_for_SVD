volatile sig_atomic_t nsignals;
static void catchsignal(int signal)
{
    nsignals++;
    return;
}
