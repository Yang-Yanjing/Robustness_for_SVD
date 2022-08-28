void av_des_crypt(AVDES *d, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt) {
    av_des_crypt_mac(d, dst, src, count, iv, decrypt, 0);
}
