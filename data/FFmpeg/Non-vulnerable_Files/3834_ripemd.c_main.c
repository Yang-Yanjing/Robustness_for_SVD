int main(void)
{
    int i, j, k;
    AVRIPEMD ctx;
    unsigned char digest[40];
    static const int lengths[4] = { 128, 160, 256, 320 };
    for (j = 0; j < 4; j++) {
        printf("Testing RIPEMD-%d\n", lengths[j]);
        for (k = 0; k < 3; k++) {
            av_ripemd_init(&ctx, lengths[j]);
            if (k == 0)
                av_ripemd_update(&ctx, "abc", 3);
            else if (k == 1)
                av_ripemd_update(&ctx, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
            else
                for (i = 0; i < 1000*1000; i++)
                    av_ripemd_update(&ctx, "a", 1);
            av_ripemd_final(&ctx, digest);
            for (i = 0; i < lengths[j] >> 3; i++)
                printf("%02X", digest[i]);
            putchar('\n');
        }
        switch (j) { 
        case 0:
            printf("c14a1219 9c66e4ba 84636b0f 69144c77\n"
                   "a1aa0689 d0fafa2d dc22e88b 49133a06\n"
                   "4a7f5723 f954eba1 216c9d8f 6320431f\n");
            break;
        case 1:
            printf("8eb208f7 e05d987a 9b044a8e 98c6b087 f15a0bfc\n"
                   "12a05338 4a9c0c88 e405a06c 27dcf49a da62eb2b\n"
                   "52783243 c1697bdb e16d37f9 7f68f083 25dc1528\n");
            break;
        case 2:
            printf("afbd6e22 8b9d8cbb cef5ca2d 03e6dba1 0ac0bc7d cbe4680e 1e42d2e9 75459b65\n"
                   "38430455 83aac6c8 c8d91285 73e7a980 9afb2a0f 34ccc36e a9e72f16 f6368e3f\n"
                   "ac953744 e10e3151 4c150d4d 8d7b6773 42e33399 788296e4 3ae4850c e4f97978\n");
            break;
        case 3:
            printf("de4c01b3 054f8930 a79d09ae 738e9230 1e5a1708 5beffdc1 b8d11671 3e74f82f a942d64c dbc4682d\n"
                   "d034a795 0cf72202 1ba4b84d f769a5de 2060e259 df4c9bb4 a4268c0e 935bbc74 70a969c9 d072a1ac\n"
                   "bdee37f4 371e2064 6b8b0d86 2dda1629 2ae36f40 965e8c85 09e63d1d bddecc50 3e2b63eb 9245bb66\n");
            break;
        }
    }
    return 0;
}
