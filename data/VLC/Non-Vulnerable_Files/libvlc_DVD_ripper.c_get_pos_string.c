}
char* get_pos_string(float pos) {
    int len;
    const char format[] = "%.3f %%";
    char *pos_string;
    pos *= 100;
    len = snprintf(NULL, 0, format, pos);
    pos_string = malloc(len);
    if(pos_string==NULL) return NULL;
    sprintf(pos_string, format, pos);
    return pos_string;
}
