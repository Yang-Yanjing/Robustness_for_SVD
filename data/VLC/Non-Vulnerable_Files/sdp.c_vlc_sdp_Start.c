}
char *vlc_sdp_Start (vlc_object_t *obj, const char *cfgpref,
                     const struct sockaddr *src, size_t srclen,
                     const struct sockaddr *addr, size_t addrlen)
{
    size_t cfglen = strlen (cfgpref);
    if (cfglen > 100)
        return NULL;
    char varname[cfglen + sizeof ("description")], *subvar = varname + cfglen;
    strcpy (varname, cfgpref);
    strcpy (subvar, "name");
    char *name = var_GetNonEmptyString (obj, varname);
    strcpy (subvar, "description");
    char *description = var_GetNonEmptyString (obj, varname);
    strcpy (subvar, "url");
    char *url = var_GetNonEmptyString (obj, varname);
    strcpy (subvar, "email");
    char *email = var_GetNonEmptyString (obj, varname);
    strcpy (subvar, "phone");
    char *phone = var_GetNonEmptyString (obj, varname);
    char *sdp = sdp_Start (name, description, url, email, phone,
                           src, srclen, addr, addrlen);
    free (name);
    free (description);
    free (url);
    free (email);
    free (phone);
    if (sdp == NULL)
        return NULL;
    strcpy (subvar, "cat");
    char *cat = var_GetNonEmptyString (obj, varname);
    if (cat != NULL)
    {
        sdp_AddAttribute (&sdp, "cat", "%s", cat);
        /* Totally non-standard */
        sdp_AddAttribute (&sdp, "x-plgroup", "%s", cat);
        free (cat);
    }
    return sdp;
}
