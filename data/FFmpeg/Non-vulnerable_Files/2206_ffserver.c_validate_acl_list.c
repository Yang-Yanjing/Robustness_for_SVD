static int validate_acl_list(FFServerIPAddressACL *in_acl, HTTPContext *c)
{
    enum FFServerIPAddressAction last_action = IP_DENY;
    FFServerIPAddressACL *acl;
    struct in_addr *src = &c->from_addr.sin_addr;
    unsigned long src_addr = src->s_addr;
    for (acl = in_acl; acl; acl = acl->next) {
        if (src_addr >= acl->first.s_addr && src_addr <= acl->last.s_addr)
            return (acl->action == IP_ALLOW) ? 1 : 0;
        last_action = acl->action;
    }
    
    return (last_action == IP_DENY) ? 1 : 0;
}
