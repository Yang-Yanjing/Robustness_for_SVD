/* Only use this with filter objects from _this_ C module */
static inline chained_filter_t *chained (filter_t *filter)
{
    return (chained_filter_t *)filter;
}
