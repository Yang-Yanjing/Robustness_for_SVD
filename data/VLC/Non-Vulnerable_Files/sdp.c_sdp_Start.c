static
char *sdp_Start (const char *name, const char *description, const char *url,
                 const char *email, const char *phone,
                 const struct sockaddr *src, size_t srclen,
                 const struct sockaddr *addr, size_t addrlen)
{
    uint64_t now = NTPtime64 ();
    char *sdp;
    char connection[MAXSDPADDRESS], hostname[256],
         sfilter[MAXSDPADDRESS + sizeof ("\r\na=source-filter: incl * ")];
    const char *preurl = "\r\nu=", *premail = "\r\ne=", *prephone = "\r\np=";
    gethostname (hostname, sizeof (hostname));
    if (name == NULL)
        name = "Unnamed";
    if (description == NULL)
        description = "N/A";
    if (url == NULL)
        preurl = url = "";
    if (email == NULL)
        premail = email = "";
    if (phone == NULL)
        prephone = phone = "";
    if (!IsSDPString (name) || !IsSDPString (description)
     || !IsSDPString (url) || !IsSDPString (email) || !IsSDPString (phone)
     || (AddressToSDP (addr, addrlen, connection) == NULL))
        return NULL;
    strcpy (sfilter, "");
    if (srclen > 0)
    {
        char machine[MAXSDPADDRESS];
        if (AddressToSDP (src, srclen, machine) != NULL)
            sprintf (sfilter, "\r\na=source-filter: incl IN IP%c * %s",
                     machine[5], machine + 7);
    }
    if (asprintf (&sdp, "v=0"
                    "\r\no=- %"PRIu64" %"PRIu64" IN IP%c %s"
                    "\r\ns=%s"
                    "\r\ni=%s"
                    "%s%s" // optional URL
                    "%s%s" // optional email
                    "%s%s" // optional phone number
                    "\r\nc=%s"
                        // bandwidth not specified
                    "\r\nt=0 0" // one dummy time span
                        // no repeating
                        // no time zone adjustment (silly idea anyway)
                        // no encryption key (deprecated)
                    "\r\na=tool:"PACKAGE_STRING
                    "\r\na=recvonly"
                    "\r\na=type:broadcast"
                    "\r\na=charset:UTF-8"
                    "%s" // optional source filter
                    "\r\n",
               /* o= */ now, now, connection[5], hostname,
               /* s= */ name,
               /* i= */ description,
               /* u= */ preurl, url,
               /* e= */ premail, email,
               /* p= */ prephone, phone,
               /* c= */ connection,
    /* source-filter */ sfilter) == -1)
        return NULL;
    return sdp;
}
