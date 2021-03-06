/*
 * Copyright (c) 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution is only permitted until one year after the first shipment
 * of 4.4BSD by the Regents.  Otherwise, redistribution and use in source and
 * binary forms are permitted provided that: (1) source distributions retain
 * this entire copyright notice and comment, and (2) distributions including
 * binaries display the following acknowledgement:  This product includes
 * software developed by the University of California, Berkeley and its
 * contributors'' in the documentation or other materials provided with the
 * distribution and in all advertising materials mentioning features or use
 * of this software.  Neither the name of the University nor the names of
 * its contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)cltp_usrreq.c	7.5 (Berkeley) 6/28/90
 */

#ifndef CLTPOVAL_SRC /* XXX -- till files gets changed */
#include "param.h"
#include "user.h"
#include "malloc.h"
#include "mbuf.h"
#include "protosw.h"
#include "socket.h"
#include "socketvar.h"
#include "errno.h"
#include "stat.h"

#include "../net/if.h"
#include "../net/route.h"

#include "argo_debug.h"
#include "iso.h"
#include "iso_pcb.h"
#include "iso_var.h"
#include "clnp.h"
#include "cltp_var.h"
#endif

/*
 * CLTP protocol implementation.
 * Per ISO 8602, December, 1987.
 */
cltp_init()
{

	cltb.isop_next = cltb.isop_prev = &cltb;
}

int cltp_cksum = 1;


/* ARGUSED */
cltp_input(m0, srcsa, dstsa, cons_channel, output)
	struct mbuf *m0;
	struct sockaddr *srcsa, *dstsa;
	u_int cons_channel;
	int (*output)();
{
	register struct isopcb *isop;
	register struct mbuf *m = m0;
	register u_char *up = mtod(m, u_char *);
	register struct sockaddr_iso *src = (struct sockaddr_iso *)srcsa;
	int len, hdrlen = *up + 1, dlen = 0;
	u_char *uplim = up + hdrlen;
	caddr_t dtsap;

	for (len = 0; m; m = m->m_next)
		len += m->m_len;
	up += 2; /* skip header */
	while (up < uplim) switch (*up) { /* process options */
	case CLTPOVAL_SRC:
		src->siso_tlen = up[1];
		src->siso_len = up[1] + TSEL(src) - (caddr_t)src;
		if (src->siso_len < sizeof(*src))
			src->siso_len = sizeof(*src);
		else if (src->siso_len > sizeof(*src)) {
			MGET(m, M_DONTWAIT, MT_SONAME);
			if (m == 0)
				goto bad;
			m->m_len = src->siso_len;
			src = mtod(m, struct sockaddr_iso *);
			bcopy((caddr_t)srcsa, (caddr_t)src, srcsa->sa_len);
		}
		bcopy((caddr_t)up + 2, TSEL(src), up[1]);
		up += 2 + src->siso_tlen;
		continue;
	
	case CLTPOVAL_DST:
		dtsap = 2 + (caddr_t)up;
		dlen = up[1];
		up += 2 + dlen;
		continue;

	case CLTPOVAL_CSM:
		if (iso_check_csum(m0, len)) {
			cltpstat.cltps_badsum++;
			goto bad;
		}
		up += 4;
		continue;

	default:
		printf("clts: unknown option (%x)\n", up[0]);
		cltpstat.cltps_hdrops++;
		goto bad;
	}
	if (dlen == 0 || src->siso_tlen == 0)
		goto bad;
	for (isop = cltb.isop_next;; isop = isop->isop_next) {
		if (isop == &cltb) {
			cltpstat.cltps_noport++;
			goto bad;
		}
		if (isop->isop_laddr &&
		    bcmp(TSEL(isop->isop_laddr), dtsap, dlen) == 0)
			break;
	}
	m = m0;
	m->m_len -= hdrlen;
	m->m_data += hdrlen;
	if (sbappendaddr(&isop->isop_socket->so_rcv, (struct sockaddr *)src,
	    m, (struct mbuf *)0) == 0)
		goto bad;
	cltpstat.cltps_ipackets++;
	sorwakeup(isop->isop_socket);
	m0 = 0;
bad:
	if (src != (struct sockaddr_iso *)srcsa)
		m_freem(dtom(src));
	if (m0)
		m_freem(m0);
	return 0;
}

/*
 * Notify a cltp user of an asynchronous error;
 * just wake up so that he can collect error status.
 */
cltp_notify(isop)
	register struct isopcb *isop;
{

	sorwakeup(isop->isop_socket);
	sowwakeup(isop->isop_socket);
}

cltp_ctlinput(cmd, sa)
	int cmd;
	struct sockaddr *sa;
{
	extern u_char inetctlerrmap[];
	struct sockaddr_iso *siso;
	int iso_rtchange();

	if ((unsigned)cmd > PRC_NCMDS)
		return;
	if (sa->sa_family != AF_ISO && sa->sa_family != AF_CCITT)
		return;
	siso = (struct sockaddr_iso *)sa;
	if (siso == 0 || siso->siso_nlen == 0)
		return;

	switch (cmd) {
	case PRC_ROUTEDEAD:
	case PRC_REDIRECT_NET:
	case PRC_REDIRECT_HOST:
	case PRC_REDIRECT_TOSNET:
	case PRC_REDIRECT_TOSHOST:
		iso_pcbnotify(&cltb, siso,
				(int)inetctlerrmap[cmd], iso_rtchange);
		break;

	default:
		if (inetctlerrmap[cmd] == 0)
			return;		/* XXX */
		iso_pcbnotify(&cltb, siso, (int)inetctlerrmap[cmd],
			cltp_notify);
	}
}

cltp_output(isop, m)
	register struct isopcb *isop;
	register struct mbuf *m;
{
	register int len;
	register struct sockaddr_iso *siso;
	int hdrlen, error = 0, docsum;
	register u_char *up;

	if (isop->isop_laddr == 0 || isop->isop_faddr == 0) {
		error = ENOTCONN;
		goto bad;
	}
	/*
	 * Calculate data length and get a mbuf for CLTP header.
	 */
	hdrlen = 2 + 2 + isop->isop_laddr->siso_tlen
		   + 2 + isop->isop_faddr->siso_tlen;
	if (docsum = /*isop->isop_flags & CLNP_NO_CKSUM*/ cltp_cksum)
		hdrlen += 4;
	M_PREPEND(m, hdrlen, M_WAIT);
	len = m->m_pkthdr.len;
	/*
	 * Fill in mbuf with extended CLTP header
	 */
	up = mtod(m, u_char *);
	up[0] = hdrlen - 1;
	up[1] = UD_TPDU_type;
	up[2] = CLTPOVAL_SRC;
	up[3] = (siso = isop->isop_laddr)->siso_tlen;
	up += 4;
	bcopy(TSEL(siso), (caddr_t)up, siso->siso_tlen);
	up += siso->siso_tlen;
	up[0] = CLTPOVAL_DST;
	up[1] = (siso = isop->isop_faddr)->siso_tlen;
	up += 2;
	bcopy(TSEL(siso), (caddr_t)up, siso->siso_tlen);
	/*
	 * Stuff checksum and output datagram.
	 */
	if (docsum) {
		up += siso->siso_tlen;
		up[0] = CLTPOVAL_CSM;
		up[1] = 2;
		iso_gen_csum(m, 2 + up - mtod(m, u_char *), len);
	}
	cltpstat.cltps_opackets++;
	return (tpclnp_output(isop, m, len, !docsum));
bad:
	m_freem(m);
	return (error);
}

#ifndef TP_LOCAL
/* XXXX should go in iso.h maybe? from tp_param.h, in any case */
#define		TP_LOCAL				22
#define		TP_FOREIGN				33
#endif

u_long	cltp_sendspace = 9216;		/* really max datagram size */
u_long	cltp_recvspace = 40 * (1024 + sizeof(struct sockaddr_iso));
					/* 40 1K datagrams */


/*ARGSUSED*/
cltp_usrreq(so, req, m, nam, control)
	struct socket *so;
	int req;
	struct mbuf *m, *nam, *control;
{
	struct isopcb *isop = sotoisopcb(so);
	int s, error = 0;

	if (req == PRU_CONTROL)
		return (iso_control(so, (int)m, (caddr_t)nam,
			(struct ifnet *)control));
	if ((isop == NULL && req != PRU_ATTACH) ||
	    (control && control->m_len)) {
		error = EINVAL;
		goto release;
	}
	switch (req) {

	case PRU_ATTACH:
		if (isop != NULL) {
			error = EINVAL;
			break;
		}
		error = iso_pcballoc(so, &cltb);
		if (error)
			break;
		error = soreserve(so, cltp_sendspace, cltp_recvspace);
		if (error)
			break;
		break;

	case PRU_DETACH:
		iso_pcbdetach(isop);
		break;

	case PRU_BIND:
		error = iso_pcbbind(isop, nam);
		break;

	case PRU_LISTEN:
		error = EOPNOTSUPP;
		break;

	case PRU_CONNECT:
		if (isop->isop_faddr) {
			error = EISCONN;
			break;
		}
		error = iso_pcbconnect(isop, nam);
		if (error == 0)
			soisconnected(so);
		break;

	case PRU_CONNECT2:
		error = EOPNOTSUPP;
		break;

	case PRU_ACCEPT:
		error = EOPNOTSUPP;
		break;

	case PRU_DISCONNECT:
		if (isop->isop_faddr == 0) {
			error = ENOTCONN;
			break;
		}
		iso_pcbdisconnect(isop);
		so->so_state &= ~SS_ISCONNECTED;		/* XXX */
		break;

	case PRU_SHUTDOWN:
		socantsendmore(so);
		break;

	case PRU_SEND:
		if (nam) {
			if (isop->isop_faddr) {
				error = EISCONN;
				break;
			}
			/*
			 * Must block input while temporarily connected.
			 */
			s = splnet();
			error = iso_pcbconnect(isop, nam);
			if (error) {
				splx(s);
				break;
			}
		} else {
			if (isop->isop_faddr == 0) {
				error = ENOTCONN;
				break;
			}
		}
		error = cltp_output(isop, m);
		m = 0;
		if (nam) {
			iso_pcbdisconnect(isop);
			splx(s);
		}
		break;

	case PRU_ABORT:
		soisdisconnected(so);
		iso_pcbdetach(isop);
		break;

	case PRU_SOCKADDR:
		iso_getnetaddr(isop, nam, TP_LOCAL);
		break;

	case PRU_PEERADDR:
		iso_getnetaddr(isop, nam, TP_FOREIGN);
		break;

	case PRU_SENSE:
		/*
		 * stat: don't bother with a blocksize.
		 */
		return (0);

	case PRU_SENDOOB:
	case PRU_FASTTIMO:
	case PRU_SLOWTIMO:
	case PRU_PROTORCV:
	case PRU_PROTOSEND:
		error =  EOPNOTSUPP;
		break;

	case PRU_RCVD:
	case PRU_RCVOOB:
		return (EOPNOTSUPP);	/* do not free mbuf's */

	default:
		panic("cltp_usrreq");
	}
release:
	if (control != NULL)
		m_freem(control);
	if (m != NULL)
		m_freem(m);
	return (error);
}
