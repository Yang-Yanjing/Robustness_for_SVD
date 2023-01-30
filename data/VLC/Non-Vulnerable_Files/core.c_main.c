}
int main (void)
{
    test_init();
    test_core (test_defaults_args, test_defaults_nargs);
    test_audiovideofilterlists (test_defaults_args, test_defaults_nargs);
    test_audio_output ();
    return 0;
}
