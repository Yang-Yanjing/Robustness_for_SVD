void av_aes_crypt(AVAES *a, uint8_t *dst, const uint8_t *src,
                  int count, uint8_t *iv, int decrypt)
{
    while (count--) {
        addkey_s(&a->state[1], src, &a->round_key[a->rounds]);
        if (decrypt) {
            aes_crypt(a, 0, inv_sbox, dec_multbl);
            if (iv) {
                addkey_s(&a->state[0], iv, &a->state[0]);
                memcpy(iv, src, 16);
            }
            addkey_d(dst, &a->state[0], &a->round_key[0]);
        } else {
            if (iv)
                addkey_s(&a->state[1], iv, &a->state[1]);
            aes_crypt(a, 2, sbox, enc_multbl);
            addkey_d(dst, &a->state[0], &a->round_key[0]);
            if (iv)
                memcpy(iv, dst, 16);
        }
        src += 16;
        dst += 16;
    }
}
