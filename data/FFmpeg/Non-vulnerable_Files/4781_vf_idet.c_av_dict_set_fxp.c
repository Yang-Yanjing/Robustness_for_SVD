static int av_dict_set_fxp(AVDictionary **pm, const char *key, uint64_t value, unsigned int digits,
                int flags)
{
    char valuestr[44];
    uint64_t print_precision = uintpow(10, digits);
    value = av_rescale(value, print_precision, PRECISION);
    snprintf(valuestr, sizeof(valuestr), "%"PRId64".%0*"PRId64,
             value / print_precision, digits, value % print_precision);
    return av_dict_set(pm, key, valuestr, flags);
}
