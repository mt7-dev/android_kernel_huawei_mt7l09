/* ASN.1 Object identifier (OID) registry
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#ifndef _LINUX_OID_REGISTRY_H
#define _LINUX_OID_REGISTRY_H

#include <linux/types.h>

/*
 * OIDs are turned into these values if possible, or OID__NR if not held here.
 *
 * NOTE!  Do not mess with the format of each line as this is read by
 *	  build_OID_registry.pl to generate the data for look_up_OID().
 */
enum OID {
	OID_id_dsa_with_sha1,		/* 1.2.840.10030.4.3 */
	OID_id_dsa,			/* 1.2.840.10040.4.1 */
	OID_id_ecdsa_with_sha1,		/* 1.2.840.10045.4.1 */
	OID_id_ecPublicKey,		/* 1.2.840.10045.2.1 */

	OID_rsaEncryption,
	OID_md2WithRSAEncryption,
	OID_md3WithRSAEncryption,
	OID_md4WithRSAEncryption,
	OID_sha1WithRSAEncryption,
	OID_sha256WithRSAEncryption,
	OID_sha384WithRSAEncryption,
	OID_sha512WithRSAEncryption,
	OID_sha224WithRSAEncryption,
	OID_data,
	OID_signed_data,
	OID_email_address,
	OID_content_type,
	OID_messageDigest,
	OID_signingTime,
	OID_smimeCapabilites,
	OID_smimeAuthenticatedAttrs,

	OID_md2,
	OID_md4,
	OID_md5,

	OID_certAuthInfoAccess,		/* 1.3.6.1.5.5.7.1.1 */
	OID_msOutlookExpress,		/* 1.3.6.1.4.1.311.16.4 */
	OID_sha1,			/* 1.3.14.3.2.26 */

	/* Distinguished Name attribute IDs [RFC 2256] */
	OID_commonName,			/* 2.5.4.3 */
	OID_surname,			/* 2.5.4.4 */
	OID_countryName,		/* 2.5.4.6 */
	OID_locality,			/* 2.5.4.7 */
	OID_stateOrProvinceName,	/* 2.5.4.8 */
	OID_organizationName,		/* 2.5.4.10 */
	OID_organizationUnitName,	/* 2.5.4.11 */
	OID_title,			/* 2.5.4.12 */
	OID_description,		/* 2.5.4.13 */
	OID_name,			/* 2.5.4.41 */
	OID_givenName,			/* 2.5.4.42 */
	OID_initials,			/* 2.5.4.43 */
	OID_generationalQualifier,	/* 2.5.4.44 */

	/* Certificate extension IDs */
	OID_subjectKeyIdentifier,	/* 2.5.29.14 */
	OID_keyUsage,			/* 2.5.29.15 */
	OID_subjectAltName,		/* 2.5.29.17 */
	OID_issuerAltName,		/* 2.5.29.18 */
	OID_basicConstraints,		/* 2.5.29.19 */
	OID_crlDistributionPoints,	/* 2.5.29.31 */
	OID_certPolicies,		/* 2.5.29.32 */
	OID_authorityKeyIdentifier,	/* 2.5.29.35 */
	OID_extKeyUsage,		/* 2.5.29.37 */

	OID__NR
};

extern enum OID look_up_OID(const void *data, size_t datasize);
extern int sprint_oid(const void *, size_t, char *, size_t);
extern int sprint_OID(enum OID, char *, size_t);

#endif /* _LINUX_OID_REGISTRY_H */
