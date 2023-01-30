static int do_adobe_auth(RTMPContext *rt, const char *user, const char *salt,
                         const char *opaque, const char *challenge)
{
    uint8_t hash[16];
    char hashstr[AV_BASE64_SIZE(sizeof(hash))], challenge2[10];
    struct AVMD5 *md5 = av_md5_alloc();
    if (!md5)
        return AVERROR(ENOMEM);
    snprintf(challenge2, sizeof(challenge2), "%08x", av_get_random_seed());
    av_md5_init(md5);
    av_md5_update(md5, user, strlen(user));
    av_md5_update(md5, salt, strlen(salt));
    av_md5_update(md5, rt->password, strlen(rt->password));
    av_md5_final(md5, hash);
    av_base64_encode(hashstr, sizeof(hashstr), hash,
                     sizeof(hash));
    av_md5_init(md5);
    av_md5_update(md5, hashstr, strlen(hashstr));
    if (opaque)
        av_md5_update(md5, opaque, strlen(opaque));
    else if (challenge)
        av_md5_update(md5, challenge, strlen(challenge));
    av_md5_update(md5, challenge2, strlen(challenge2));
    av_md5_final(md5, hash);
    av_base64_encode(hashstr, sizeof(hashstr), hash,
                     sizeof(hash));
    snprintf(rt->auth_params, sizeof(rt->auth_params),
             "?authmod=%s&user=%s&challenge=%s&response=%s",
             "adobe", user, challenge2, hashstr);
    if (opaque)
        av_strlcatf(rt->auth_params, sizeof(rt->auth_params),
                    "&opaque=%s", opaque);
    av_free(md5);
    return 0;
}
