Any error will currently print output to stderr.  This will eventually be replaced with better 
	output in the Exceptions that are also raised.

The DTLS1 bindings are not usable unless the patches in this file are applied to OpenSSL 0.9.8e.
	However, this patch includes a deactivation of the handshake retransmission code in d1_both.c, 
	making it unsuitable for production environments.  I take no responsibility, etc, etc. 
	If you want to use it anyway, apply the patches(gathered from various newsgroups and my own
	experimentation) and uncomment the commented-out block of build.io.
	For what it's worth, DTLS support in OpenSSL is new as of 0.9.8 and is pretty buggy to begin
	with.  It's a nice idea, but it doesn't seem to be production ready at all yet.  These bindings
	are no exception.

diff -r openssl-clean-0.9.8e/ssl/d1_both.c openssl-0.9.8e/ssl/d1_both.c
728,735c728,735
<     overlap = (s->init_num - DTLS1_HM_HEADER_LENGTH) - frag_off;
<         
<     /* retain the header for the first fragment */
<     if ( s->init_num > DTLS1_HM_HEADER_LENGTH)
<         {
<         memmove(&(s->init_buf->data[s->init_num]),
<             &(s->init_buf->data[s->init_num + DTLS1_HM_HEADER_LENGTH + overlap]),
<             frag_len - overlap);
---
> 	if ( s->init_num > DTLS1_HM_HEADER_LENGTH)
> 	{
> 		s->init_num -= DTLS1_HM_HEADER_LENGTH;
> 		overlap = (s->init_num - DTLS1_HM_HEADER_LENGTH) - frag_off;
> 		
> 		memmove(&(s->init_buf->data[s->init_num]),
> 			&(s->init_buf->data[s->init_num + DTLS1_HM_HEADER_LENGTH + overlap]),
> 			frag_len - overlap);
737,740c737,743
<         s->init_num += frag_len - overlap;
<         }
<     else
<         s->init_num += frag_len;
---
> 		s->init_num += frag_len - overlap;
> 	}
> 	else
> 	{
> 		overlap = 0;
> 		s->init_num += frag_len;
> 	}
974,975c977,987
< 
<     return dtls1_retransmit_buffered_messages(s) ;
---
> /*
> In non-blocking mode, this retransmit gets spammed, which freaks out the server.  Changing 
> to always reporting the timeout fixes that case when no packets are going to be lost, but
> I hesitate to claim it fixed, since it means that retransmits are busted.
> 
> What should happen is that the timeout and retransmits are somehow application-managed 
> in non-blocking modes, but I'm not sure where to start poking at that.  ask nagendra?
> */
> 			return -1;
> //  int ret = dtls1_retransmit_buffered_messages(s) ;
> //	return ret;
diff -r openssl-clean-0.9.8e/ssl/d1_pkt.c openssl-0.9.8e/ssl/d1_pkt.c
577a578
> 			rr->length = 0;
590a592
> 		rr->length = 0;
597a600
> 		rr->length = 0;
diff -r openssl-clean-0.9.8e/ssl/d1_srvr.c openssl-0.9.8e/ssl/d1_srvr.c
1012c1012,1013
< 
---
> 	unsigned int msg_len;
> 	
1088c1089,1091
< 		/* XDTLS:  set message header ? */
---
> 		msg_len = s->init_num - DTLS1_HM_HEADER_LENGTH;
> 		dtls1_set_message_header(s, s->init_buf->data, 
> 			SSL3_MT_CERTIFICATE_REQUEST, msg_len, 0, msg_len);