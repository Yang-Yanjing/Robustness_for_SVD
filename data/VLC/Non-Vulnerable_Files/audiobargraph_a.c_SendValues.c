}
static void SendValues(filter_t *p_filter, float *value, int nbChannels)
{
    char msg[256];
    size_t len = 0;
    for (int i = 0; i < nbChannels; i++) {
        if (len >= sizeof (msg))
            break;
        len += snprintf(msg + len, sizeof (msg) - len, "%f:", value[i]);
    }
    //msg_Dbg(p_filter, "values: %s", message);
    var_SetString(p_filter->p_libvlc, "audiobargraph_v-i_values", msg);
}
