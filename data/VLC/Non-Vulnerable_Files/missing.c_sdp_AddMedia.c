# include <vlc_sout.h>
char *sdp_AddMedia (char **sdp, const char *type, const char *protocol,
                    int dport, unsigned pt, bool bw_indep, unsigned bw,
                    const char *ptname, unsigned clockrate, unsigned channels,
                    const char *fmtp)
{
    VLC_UNUSED (sdp); VLC_UNUSED (type); VLC_UNUSED (protocol);
    VLC_UNUSED (dport); VLC_UNUSED (pt); VLC_UNUSED (bw_indep);
    VLC_UNUSED (bw); VLC_UNUSED (ptname); VLC_UNUSED (clockrate);
    VLC_UNUSED (channels); VLC_UNUSED (fmtp);
    assert (*sdp == NULL);
}
