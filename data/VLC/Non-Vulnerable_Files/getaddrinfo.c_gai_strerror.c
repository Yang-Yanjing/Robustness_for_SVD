 ****************************************************************************/
const char *gai_strerror (int errnum)
{
    for (unsigned i = 0; *gai_errlist[i].msg; i++)
        if (errnum == gai_errlist[i].code)
            return gai_errlist[i].msg;
    return gai_unknownerr;
}
