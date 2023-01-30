double av_strtod(const char *numstr, char **tail)
{
    double d;
    char *next;
    if(numstr[0]=='0' && (numstr[1]|0x20)=='x') {
        d = strtoul(numstr, &next, 16);
    } else
        d = strtod(numstr, &next);
    
    if (next!=numstr) {
        if (next[0] == 'd' && next[1] == 'B') {
            
            d = pow(10, d / 20);
            next += 2;
        } else if (*next >= 'E' && *next <= 'z') {
            int e= si_prefixes[*next - 'E'];
            if (e) {
                if (next[1] == 'i') {
                    d*= pow( 2, e/0.3);
                    next+=2;
                } else {
                    d*= pow(10, e);
                    next++;
                }
            }
        }
        if (*next=='B') {
            d*=8;
            next++;
        }
    }
    

    if (tail)
        *tail = next;
    return d;
}
