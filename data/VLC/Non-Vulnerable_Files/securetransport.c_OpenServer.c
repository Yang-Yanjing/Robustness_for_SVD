 */
static int OpenServer (vlc_tls_creds_t *crd, const char *cert, const char *key) {
    /*
     * This function expects the label of the certificate in "cert", stored
     * in the MacOS keychain. The appropriate private key is found automatically.
     */
    VLC_UNUSED(key);
    OSStatus ret;
    msg_Dbg(crd, "open st server");
    /*
     * Get the server certificate.
     *
     * This API is deprecated, but the replacement SecItemCopyMatching
     * only works on >= 10.7
     */
    SecKeychainAttribute attrib = { kSecLabelItemAttr, strlen(cert), (void *)cert };
    SecKeychainAttributeList attrList = { 1, &attrib };
    SecKeychainSearchRef searchReference = NULL;
    ret = SecKeychainSearchCreateFromAttributes(NULL, kSecCertificateItemClass,
                                                 &attrList, &searchReference);
    if (ret != noErr || searchReference == NULL) {
        msg_Err(crd, "Cannot find certificate with alias %s", cert);
        return VLC_EGENERIC;
    }
    SecKeychainItemRef itemRef = NULL;
    ret = SecKeychainSearchCopyNext(searchReference, &itemRef);
    if (ret != noErr) {
        msg_Err(crd, "Cannot get certificate with alias %s, error: %d", cert, ret);
        return VLC_EGENERIC;
    }
    CFRelease(searchReference);
    /* cast allowed according to documentation */
    SecCertificateRef certificate = (SecCertificateRef)itemRef;
    SecIdentityRef cert_identity = NULL;
    ret = SecIdentityCreateWithCertificate(NULL, certificate, &cert_identity);
    if (ret != noErr) {
        msg_Err(crd, "Cannot get private key for certificate");
        CFRelease(certificate);
        return VLC_EGENERIC;
    }
    /*
     * We try to validate the server certificate, but do not care about the result.
     * The only aim is to get the certificate chain.
     */
    SecPolicyRef policy = SecPolicyCreateSSL(true, NULL);
    SecTrustRef trust_ref = NULL;
    int result = VLC_SUCCESS;
    /* According to docu its fine to pass just one certificate */
    ret = SecTrustCreateWithCertificates((CFArrayRef)certificate, policy, &trust_ref);
    if (ret != noErr) {
        msg_Err(crd, "Cannot create trust");
        result = VLC_EGENERIC;
        goto out;
    }
    SecTrustResultType status;
    ret = SecTrustEvaluate(trust_ref, &status);
    if (ret != noErr) {
        msg_Err(crd, "Error evaluating trust");
        result = VLC_EGENERIC;
        goto out;
    }
    CFArrayRef cert_chain = NULL;
    CSSM_TP_APPLE_EVIDENCE_INFO *status_chain;
    ret = SecTrustGetResult(trust_ref, &status, &cert_chain, &status_chain);
    if (ret != noErr || !cert_chain) {
        msg_Err(crd, "error while getting certificate chain");
        result = VLC_EGENERIC;
        goto out;
    }
    CFIndex num_cert_chain = CFArrayGetCount(cert_chain);
    /* Build up the certificate chain array expected by SSLSetCertificate */
    CFMutableArrayRef server_cert_chain = CFArrayCreateMutable(kCFAllocatorDefault, num_cert_chain, &kCFTypeArrayCallBacks);
    CFArrayAppendValue(server_cert_chain, cert_identity);
    msg_Dbg(crd, "Found certificate chain with %ld entries for server certificate", num_cert_chain);
    if (num_cert_chain > 1)
        CFArrayAppendArray(server_cert_chain, cert_chain, CFRangeMake(1, num_cert_chain - 1));
    CFRelease(cert_chain);
    vlc_tls_creds_sys_t *sys = malloc(sizeof(*sys));
    if (unlikely(sys == NULL)) {
        CFRelease(server_cert_chain);
        result = VLC_ENOMEM;
        goto out;
    }
    sys->server_cert_chain = server_cert_chain;
    sys->whitelist = NULL;
    crd->sys = sys;
    crd->open = st_ServerSessionOpen;
    crd->close = st_SessionClose;
out:
    if (policy)
        CFRelease(policy);
    if (trust_ref)
        CFRelease(trust_ref);
    if (certificate)
        CFRelease(certificate);
    if (cert_identity)
        CFRelease(cert_identity);
    return result;
}
