static size_t htmlencode (const char *src, char **dest) {
    const char *amp = "&amp;";
    const char *lt  = "&lt;";
    const char *gt  = "&gt;";
    const char *start;
    char *tmp;
    size_t final_size = 0;
    if (!src)
        return 0;
    start = src;
    
    while (*src != '\0') {
        switch(*src) {
            case 38: 
                final_size += 5;
                break;
            case 60: 
            case 62: 
                final_size += 4;
                break;
            default:
                final_size++;
        }
        src++;
    }
    src = start;
    *dest = av_mallocz(final_size + 1);
    if (!*dest)
        return 0;
    
    tmp = *dest;
    while (*src != '\0') {
        switch(*src) {
            case 38: 
                cp_html_entity (tmp, amp);
                tmp += 5;
                break;
            case 60: 
                cp_html_entity (tmp, lt);
                tmp += 4;
                break;
            case 62: 
                cp_html_entity (tmp, gt);
                tmp += 4;
                break;
            default:
                *tmp = *src;
                tmp += 1;
        }
        src++;
    }
    *tmp = '\0';
    return final_size;
}
