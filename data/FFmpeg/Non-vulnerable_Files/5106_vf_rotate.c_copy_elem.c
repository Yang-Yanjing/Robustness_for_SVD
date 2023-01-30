static av_always_inline void copy_elem(uint8_t *pout, const uint8_t *pin, int elem_size)
{
    int v;
    switch (elem_size) {
    case 1:
        *pout = *pin;
        break;
    case 2:
        *((uint16_t *)pout) = *((uint16_t *)pin);
        break;
    case 3:
        v = AV_RB24(pin);
        AV_WB24(pout, v);
        break;
    case 4:
        *((uint32_t *)pout) = *((uint32_t *)pin);
        break;
    default:
        memcpy(pout, pin, elem_size);
        break;
    }
}
