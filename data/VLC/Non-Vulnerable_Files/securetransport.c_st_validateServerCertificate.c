}
static int st_validateServerCertificate (vlc_tls_t *session, const char *hostname) {
    int result = -1;
    vlc_tls_sys_t *sys = session->sys;
    SecCertificateRef leaf_cert = NULL;
    SecTrustRef trust = NULL;
    OSStatus ret = SSLCopyPeerTrust(sys->p_context, &trust);
    if (ret != noErr || trust == NULL) {
        msg_Err(session, "error getting certifictate chain");
        return -1;
    }
    CFStringRef cfHostname = CFStringCreateWithCString(kCFAllocatorDefault,
                                                       hostname,
                                                       kCFStringEncodingUTF8);
    /* enable default root / anchor certificates */
    ret = SecTrustSetAnchorCertificates(trust, NULL);
    if (ret != noErr) {
        msg_Err(session, "error setting anchor certificates");
        result = -1;
        goto out;
    }
    SecTrustResultType trust_eval_result = 0;
    ret = SecTrustEvaluate(trust, &trust_eval_result);
    if (ret != noErr) {
        msg_Err(session, "error calling SecTrustEvaluate");
        result = -1;
        goto out;
    }
    switch (trust_eval_result) {
        case kSecTrustResultUnspecified:
        case kSecTrustResultProceed:
            msg_Dbg(session, "cerfificate verification successful, result is %d", trust_eval_result);
            result = 0;
            goto out;
        case kSecTrustResultRecoverableTrustFailure:
        case kSecTrustResultDeny:
        default:
            msg_Warn(session, "cerfificate verification failed, result is %d", trust_eval_result);
    }
    /* get leaf certificate */
    /* SSLCopyPeerCertificates is only available on OSX 10.5 or later */
#if !TARGET_OS_IPHONE
    CFArrayRef cert_chain = NULL;
    ret = SSLCopyPeerCertificates(sys->p_context, &cert_chain);
    if (ret != noErr || !cert_chain) {
        result = -1;
        goto out;
    }
    if (CFArrayGetCount(cert_chain) == 0) {
        CFRelease(cert_chain);
        result = -1;
        goto out;
    }
    leaf_cert = (SecCertificateRef)CFArrayGetValueAtIndex(cert_chain, 0);
    CFRetain(leaf_cert);
    CFRelease(cert_chain);
#else
    /* SecTrustGetCertificateAtIndex is only available on 10.7 or iOS */
    if (SecTrustGetCertificateCount(trust) == 0) {
        result = -1;
        goto out;
    }
    leaf_cert = SecTrustGetCertificateAtIndex(trust, 0);
    CFRetain(leaf_cert);
#endif
    /* check if leaf already accepted */
    CFIndex max = CFArrayGetCount(sys->p_cred->whitelist);
    for (CFIndex i = 0; i < max; ++i) {
        CFDictionaryRef dict = CFArrayGetValueAtIndex(sys->p_cred->whitelist, i);
        CFStringRef knownHost = (CFStringRef)CFDictionaryGetValue(dict, cfKeyHost);
        SecCertificateRef knownCert = (SecCertificateRef)CFDictionaryGetValue(dict, cfKeyCertificate);
        if (!knownHost || !knownCert)
            continue;
        if (CFEqual(knownHost, cfHostname) && CFEqual(knownCert, leaf_cert)) {
            msg_Warn(session, "certificate already accepted, continuing");
            result = 0;
            goto out;
        }
    }
    /* We do not show more certificate details yet because there is no proper API to get
       a summary of the certificate. SecCertificateCopySubjectSummary is the only method
       available on iOS and 10.6. More promising API functions such as
       SecCertificateCopyLongDescription also print out the subject only, more or less.
       But only showing the certificate subject is of no real help for the user.
       We could use SecCertificateCopyValues, but then we need to parse all OID values for
       ourself. This is too mad for just printing information the user will never check
       anyway.
     */
    const char *msg = N_("You attempted to reach %s. "
             "However the security certificate presented by the server "
             "is unknown and could not be authenticated by any trusted "
             "Certification Authority. "
             "This problem may be caused by a configuration error "
             "or an attempt to breach your security or your privacy.\n\n"
             "If in doubt, abort now.\n");
    int answer = dialog_Question(session, _("Insecure site"), vlc_gettext (msg),
                                  _("Abort"), _("Accept certificate temporarily"), NULL, hostname);
    if (answer == 2) {
        msg_Warn(session, "Proceeding despite of failed certificate validation");
        /* save leaf certificate in whitelist */
        const void *keys[] = {cfKeyHost, cfKeyCertificate};
        const void *values[] = {cfHostname, leaf_cert};
        CFDictionaryRef dict = CFDictionaryCreate(kCFAllocatorDefault,
                                                   keys, values, 2,
                                                   &kCFTypeDictionaryKeyCallBacks,
                                                   &kCFTypeDictionaryValueCallBacks);
        if (!dict) {
            msg_Err(session, "error creating dict");
            result = -1;
            goto out;
        }
        CFArrayAppendValue(sys->p_cred->whitelist, dict);
        CFRelease(dict);
        result = 0;
        goto out;
    } else {
        result = -1;
        goto out;
    }
out:
    CFRelease(trust);
    if (cfHostname)
        CFRelease(cfHostname);
    if (leaf_cert)
        CFRelease(leaf_cert);
    return result;
}
