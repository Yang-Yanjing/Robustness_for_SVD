static void finish(void)
{
    char res[2 * AV_HASH_MAX_SIZE + 4];
    printf("%s=", av_hash_get_name(hash));
    if (out_b64) {
        av_hash_final_b64(hash, res, sizeof(res));
        printf("b64:%s", res);
    } else {
        av_hash_final_hex(hash, res, sizeof(res));
        printf("0x%s", res);
    }
}
