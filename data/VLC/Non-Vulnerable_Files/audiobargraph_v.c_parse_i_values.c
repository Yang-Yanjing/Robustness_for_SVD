 *****************************************************************************/
static void parse_i_values(BarGraph_t *p_BarGraph, char *i_values)
{
    char delim[] = ":";
    char* tok;
    p_BarGraph->nbChannels = 0;
    free(p_BarGraph->i_values);
    p_BarGraph->i_values = NULL;
    char *res = strtok_r(i_values, delim, &tok);
    while (res != NULL) {
        p_BarGraph->nbChannels++;
        p_BarGraph->i_values = xrealloc(p_BarGraph->i_values,
                                          p_BarGraph->nbChannels*sizeof(int));
        float db = log10(atof(res)) * 20;
        p_BarGraph->i_values[p_BarGraph->nbChannels-1] = VLC_CLIP(iec_scale(db)*p_BarGraph->scale, 0, p_BarGraph->scale);
        res = strtok_r(NULL, delim, &tok);
    }
}
