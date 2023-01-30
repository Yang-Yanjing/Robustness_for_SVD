 */
static vlc_log_t *msg_Copy (const vlc_log_t *msg)
{
    vlc_log_t *copy = (vlc_log_t *)xmalloc (sizeof (*copy));
    copy->i_object_id = msg->i_object_id;
    copy->psz_object_type = msg->psz_object_type;
    copy->psz_module = strdup (msg->psz_module);
    copy->psz_header = msg->psz_header ? strdup (msg->psz_header) : NULL;
    return copy;
}
