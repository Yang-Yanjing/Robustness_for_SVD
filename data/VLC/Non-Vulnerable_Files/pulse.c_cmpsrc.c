 */
static int cmpsrc (const void *a, const void *b)
{
    const uint32_t *pa = a, *pb = b;
    uint32_t idxa = *pa, idxb = *pb;
    return (idxa != idxb) ? ((idxa < idxb) ? -1 : +1) : 0;
}
