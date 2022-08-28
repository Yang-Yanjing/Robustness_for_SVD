static int do_llnw_auth(RTMPContext *rt, const char *user, const char *nonce)
{
    uint8_t hash[16];
    char hashstr1[33], hashstr2[33];
    const char *realm = "live";
    const char *method = "publish";
    const char *qop = "auth";
    const char *nc = "00000001";
    char cnonce[10];
    struct AVMD5 *md5 = av_md5_alloc();
    if (!md5)
        return AVERROR(ENOMEM);
    snprintf(cnonce, sizeof(cnonce), "%08x", av_get_random_seed());
    av_md5_init(md5);
    av_md5_update(md5, user, strlen(user));
    av_md5_update(md5, ":", 1);
    av_md5_update(md5, realm, strlen(realm));
    av_md5_update(md5, ":", 1);
    av_md5_update(md5, rt->password, strlen(rt->password));
    av_md5_final(md5, hash);
    ff_data_to_hex(hashstr1, hash, 16, 1);
    hashstr1[32] = '\0';
    av_md5_init(md5);
    av_md5_update(md5, method, strlen(method));
    av_md5_update(md5, ":/", 2);
    av_md5_update(md5, rt->app, strlen(rt->app));
    if (!strchr(rt->app, '/'))
        av_md5_update(md5, "/_definst_", strlen("/_definst_"));
    av_md5_final(md5, hash);
    ff_data_to_hex(hashstr2, hash, 16, 1);
    hashstr2[32] = '\0';
    av_md5_init(md5);
    av_md5_update(md5, hashstr1, strlen(hashstr1));
    av_md5_update(md5, ":", 1);
    if (nonce)
        av_md5_update(md5, nonce, strlen(nonce));
    av_md5_update(md5, ":", 1);
    av_md5_update(md5, nc, strlen(nc));
    av_md5_update(md5, ":", 1);
    av_md5_update(md5, cnonce, strlen(cnonce));
    av_md5_update(md5, ":", 1);
    av_md5_update(md5, qop, strlen(qop));
    av_md5_update(md5, ":", 1);
    av_md5_update(md5, hashstr2, strlen(hashstr2));
    av_md5_final(md5, hash);
    ff_data_to_hex(hashstr1, hash, 16, 1);
    snprintf(rt->auth_params, sizeof(rt->auth_params),
             "?authmod=%s&user=%s&nonce=%s&cnonce=%s&nc=%s&response=%s",
             "llnw", user, nonce, cnonce, nc, hashstr1);
    av_free(md5);
    return 0;
}
