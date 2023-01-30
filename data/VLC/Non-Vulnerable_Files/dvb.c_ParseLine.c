 * Read the dvb-apps source code for reference. */
static input_item_t *ParseLine(char *line)
{
    char *str, *end;
    line += strspn(line, " \t\r"); /* skip leading white spaces */
    if (*line == '#')
        return NULL; /* skip comments */
    /* Extract channel cute name */
    char *name = strsep(&line, ":");
    assert(name != NULL);
    EnsureUTF8(name);
    /* Extract central frequency */
    str = strsep(&line, ":");
    if (str == NULL)
        return NULL;
    unsigned long freq = strtoul(str, &end, 10);
    if (*end)
        return NULL;
    /* Extract tuning parameters */
    str = strsep(&line, ":");
    if (str == NULL)
        return NULL;
    char *mrl;
    if (!strcmp(str, "h") || !strcmp(str, "v"))
    {   /* DVB-S */
        char polarization = toupper(*str);
        /* TODO: sat no. */
        str = strsep(&line, ":");
        if (str == NULL)
            return NULL;
        /* baud rate */
        str = strsep(&line, ":");
        if (str == NULL)
            return NULL;
        unsigned long rate = strtoul(str, &end, 10);
        if (*end || rate > (ULONG_MAX / 1000u))
            return NULL;
        rate *= 1000;
        if (asprintf(&mrl,
                     "dvb-s://frequency=%"PRIu64":polarization=%c:srate=%lu",
                     freq * UINT64_C(1000000), polarization, rate) == -1)
            mrl = NULL;
    }
    else
    if (!strncmp(str, "INVERSION_", 10))
    {   /* DVB-C or DVB-T */
        int inversion;
        str += 10;
        if (strcmp(str, "AUTO"))
            inversion = -1;
        else if (strcmp(str, "OFF"))
            inversion = 0;
        else if (strcmp(str, "ON"))
            inversion = 1;
        else
            return NULL;
        str = strsep(&line, ":");
        if (str == NULL)
            return NULL;
        if (strncmp(str, "BANDWIDTH_", 10))
        {   /* DVB-C */
            unsigned long rate = strtoul(str, &end, 10);
            if (*end)
                return NULL;
            str = strsep(&line, ":");
            const char *fec = ParseFEC(str);
            str = strsep(&line, ":");
            const char *mod = ParseModulation(str);
            if (fec == NULL || mod == NULL)
                return NULL;
            if (asprintf(&mrl, "dvb-c://frequency=%lu:inversion:%d:srate=%lu:"
                         "fec=%s:modulation=%s", freq, inversion, rate, fec,
                         mod) == -1)
                mrl = NULL;
        }
        else
        {   /* DVB-T */
            unsigned bandwidth = atoi(str + 10);
            str = strsep(&line, ":");
            const char *hp = ParseFEC(str);
            str = strsep(&line, ":");
            const char *lp = ParseFEC(str);
            str = strsep(&line, ":");
            const char *mod = ParseModulation(str);
            if (hp == NULL || lp == NULL || mod == NULL)
                return NULL;
            str = strsep(&line, ":");
            if (str == NULL || strncmp(str, "TRANSMISSION_MODE_", 18))
                return NULL;
            int xmit = atoi(str);
            if (xmit == 0)
                xmit = -1; /* AUTO */
            str = strsep(&line, ":");
            const char *guard = ParseGuard(str);
            if (guard == NULL)
                return NULL;
            str = strsep(&line, ":");
            if (str == NULL || strncmp(str, "HIERARCHY_", 10))
                return NULL;
            str += 10;
            int hierarchy = atoi(str);
            if (!strcmp(str, "AUTO"))
                hierarchy = -1;
            if (asprintf(&mrl, "dvb-t://frequency=%lu:inversion=%d:"
                         "bandwidth=%u:code-rate-hp=%s:code-rate-lp=%s:"
                         "modulation=%s:transmission=%d:guard=%s:"
                         "hierarchy=%d", freq, inversion, bandwidth, hp, lp,
                         mod, xmit, guard, hierarchy) == -1)
                mrl = NULL;
        }
    }
    else
    {   /* ATSC */
        const char *mod = ParseModulation(str);
        if (mod == NULL)
            return NULL;
        if (asprintf(&mrl, "atsc://frequency=%lu:modulation=%s", freq,
                     mod) == -1)
            mrl = NULL;
    }
    if (unlikely(mrl == NULL))
        return NULL;
    /* Video PID (TODO? set video track) */
    strsep(&line, ":");
    /* Audio PID (TODO? set audio track) */
    strsep(&line, ":");
    /* Extract SID */
    str = strsep(&line, ":");
    if (str == NULL)
    {
        free(mrl);
        return NULL;
    }
    unsigned long sid = strtoul(str, &end, 10);
    if (*end || sid > 65535)
    {
        free(mrl);
        return NULL;
    }
    char sid_opt[sizeof("program=65535")];
    snprintf(sid_opt, sizeof(sid_opt), "program=%lu", sid);
    const char *opts[] = { sid_opt };
    input_item_t *item = input_item_NewWithType(mrl, name, 1, opts, 0, -1,
                                                ITEM_TYPE_CARD);
    free(mrl);
    return item;
}
