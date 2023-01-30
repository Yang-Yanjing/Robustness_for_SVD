}
static void msg_Free (vlc_log_t *msg)
{
    free ((char *)msg->psz_module);
    free ((char *)msg->psz_header);
    free (msg);
}
