}
static int filter(access_t *p_access, const char *in, const char *filter, char **out, size_t outlen) {
  int flen=strlen(filter);
  size_t len;
  if (!in) return 0;
  len = (strchr(in,'\n')) ? (size_t)(strchr(in,'\n')-in) : strlen(in);
  if (!strncmp(in,filter,flen)) {
    if(in[flen]=='"') flen++;
    if(in[len-1]==13) len--;
    if(in[len-1]=='"') len--;
    if( len-flen+1 > outlen )
    {
        msg_Warn(p_access, "Discarding end of string to avoid overflow");
        len=outlen+flen-1;
    }
    memcpy(*out, in+flen, len-flen+1);
    (*out)[len-flen]=0;
    return len-flen;
  }
  return 0;
}
