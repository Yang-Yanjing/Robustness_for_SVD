int av_des_init(AVDES *d, const uint8_t *key, int key_bits, av_unused int decrypt) {
    if (key_bits != 64 && key_bits != 192)
        return -1;
    d->triple_des = key_bits > 64;
    gen_roundkeys(d->round_keys[0], AV_RB64(key));
    if (d->triple_des) {
        gen_roundkeys(d->round_keys[1], AV_RB64(key +  8));
        gen_roundkeys(d->round_keys[2], AV_RB64(key + 16));
    }
    return 0;
}
