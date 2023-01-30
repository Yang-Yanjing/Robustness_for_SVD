 */
static int recv_fd (int p)
{
    struct msghdr hdr;
    struct iovec iov;
    struct cmsghdr *cmsg;
    int val, fd;
    char buf[CMSG_SPACE (sizeof (fd))];
    hdr.msg_name = NULL;
    hdr.msg_namelen = 0;
    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_control = buf;
    hdr.msg_controllen = sizeof (buf);
    iov.iov_base = &val;
    iov.iov_len = sizeof (val);
    if (recvmsg (p, &hdr, 0) != sizeof (val))
        return -1;
    for (cmsg = CMSG_FIRSTHDR (&hdr); cmsg != NULL;
         cmsg = CMSG_NXTHDR (&hdr, cmsg))
    {
        if ((cmsg->cmsg_level == SOL_SOCKET)
         && (cmsg->cmsg_type == SCM_RIGHTS)
         && (cmsg->cmsg_len >= CMSG_LEN (sizeof (fd))))
        {
            memcpy (&fd, CMSG_DATA (cmsg), sizeof (fd));
            return fd;
        }
    }
    errno = val;
    return -1;
}
