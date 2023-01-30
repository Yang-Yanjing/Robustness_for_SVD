}
void vlc_enable_override (void)
{
    override = true;
    pthread_atfork (NULL, NULL, vlc_reset_override);
}
#else
void vlc_enable_override (void)
{
}
