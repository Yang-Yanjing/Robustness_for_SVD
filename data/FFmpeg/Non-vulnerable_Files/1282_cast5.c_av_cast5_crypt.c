void av_cast5_crypt(AVCAST5* cs, uint8_t* dst, const uint8_t* src, int count, int decrypt)
{
    while (count--) {
        if (decrypt){
            decipher(cs, dst, src, NULL);
        } else {
            encipher(cs, dst, src);
        }
        src = src + 8;
        dst = dst + 8;
    }
}
