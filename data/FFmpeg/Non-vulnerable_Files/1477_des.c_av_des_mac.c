void av_des_mac(AVDES *d, uint8_t *dst, const uint8_t *src, int count) {
    av_des_crypt_mac(d, dst, src, count, (uint8_t[8]){0}, 0, 1);
}
}
void av_des_mac(AVDES *d, uint8_t *dst, const uint8_t *src, int count) {
    av_des_crypt_mac(d, dst, src, count, (uint8_t[8]){0}, 0, 1);
}
