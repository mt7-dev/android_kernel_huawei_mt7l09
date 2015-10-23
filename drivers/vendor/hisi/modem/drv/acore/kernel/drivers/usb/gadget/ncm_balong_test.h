/*************************************************************************
*   版权所有(C) 1987-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  ncm_balong.h
*
*   作    者 :  wangzhongshun
*
*   描    述 :  NCM测试头文件
*
*   修改记录 :  2013年2月16日  v1.00  wangzhongshun  创建

*************************************************************************/
#ifndef __NCM_BALONG_TEST_H__
#define __NCM_BALONG_TEST_H__

#ifndef _TYPES_UTIL_H_
#define _TYPES_UTIL_H_

#define CONFIG_BYTE_ORDER CPU_LITTLE_ENDIAN

/*
 *      Generic comparison and types
 */
#ifndef MIN
# define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
# define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#ifndef ABS
# define ABS(a)  ((a) < 0 ? (-(a)) : (a))
#endif

#ifndef TRUE
# define TRUE    1
#endif

#ifndef FALSE
# define FALSE   0
#endif

#define UW_CONCAT(a,b)   a ## b

#ifndef CONFIG_BYTE_ORDER
# error "CONFIG_BYTE_ORDER for this configuration is not specified !"
#endif

#define CPU_LITTLE_ENDIAN       1234 /* LSB first: i386, vax */
#define CPU_BIG_ENDIAN          4321 /* MSB first: 68000, ibm, net */

/*
 *      Unaligned access macors.
 *      Used to access PACKED structures and un-aligned integers
 */

#define ua_get_le16(ptr)        UW_CONVERT_LE(get16, ptr, 0)
#define ua_get_le32(ptr)        UW_CONVERT_LE(get32, ptr, 0)
#define ua_set_le16(ptr, val)   UW_CONVERT_LE(set16, ptr, val)
#define ua_set_le32(ptr, val)   UW_CONVERT_LE(set32, ptr, val)

#define ua_get_be16(ptr)        UW_CONVERT_BE(get16, ptr, 0)
#define ua_get_be32(ptr)        UW_CONVERT_BE(get32, ptr, 0)
#define ua_set_be16(ptr, val)   UW_CONVERT_BE(set16, ptr, val)
#define ua_set_be32(ptr, val)   UW_CONVERT_BE(set32, ptr, val)

/*
 *      Endianess handling macros
 */

#define uw_letoh16(val)            uw_le16(val)
#define uw_letoh32(val)            uw_le32(val)
#define uw_htole16(val)            uw_le16(val)
#define uw_htole32(val)            uw_le32(val)

#define uw_betoh16(val)            uw_be16(val)
#define uw_betoh32(val)            uw_be32(val)
#define uw_htobe16(val)            uw_be16(val)
#define uw_htobe32(val)            uw_be32(val)

/*
 *      Time structure handling macros
 */

/* Add "ms" milli-seconds to the time structure */
#define TIME_ADD_MS(time, ms) \
        do { \
        (time)->tv_usec += ms * 1000; \
        /* BEGIN: Modified by Mabinjie at 20110517 */ \
        /* (time)->tv_sec  += ((time)->tv_usec / (1000 * 1000)); \ */ \
        /* (time)->tv_usec %= (1000 * 1000); \ */ \
        (time)->tv_sec  += ((time)->tv_usec / (CONFIG_TIME_DIVISOR * CONFIG_TIME_DIVISOR)); \
        (time)->tv_usec %= (CONFIG_TIME_DIVISOR * CONFIG_TIME_DIVISOR); \
        /* END:   Modified by Mabinjie at 20110517 */ \
        } while (0);

/* Check if time "a" is above time "b" */
#define TIME_OVER(a, b) \
        (((a).tv_sec > (b).tv_sec) || \
        (((a).tv_sec == (b).tv_sec) && \
        ((a).tv_usec >= (b).tv_usec)) ? \
        1 : 0)

/* Check if time "a" is above time "b" with millisecond resolution */
#define TIME_OVER_MS(a, b) \
        ((TIME_DIFF(a, b) > 0) ? 0 : 1)

/* Return time difference between "start" and "end" in milli-seconds */
#define TIME_DIFF(start, end) \
        (((end).tv_sec - (start).tv_sec) * 1000 + \
        /* BEGIN: Modified by Mabinjie at 20110517 */ \
        /* ((end).tv_usec - (start).tv_usec) / 1000) */ \
        ((end).tv_usec - (start).tv_usec) / CONFIG_TIME_DIVISOR)
        /* END:   Modified by Mabinjie at 20110517 */

/* Return time difference between "start" and "end" in milli-seconds */
#define TIME_DIFF_US(start, end) \
        (((end).tv_sec - (start).tv_sec) * 1000000 + \
        (end).tv_usec - (start).tv_usec)

#define TIME_GET_MS(time) \
        /* BEGIN: Modified by Mabinjie at 20110517 */ \
        /* (((time).tv_sec) * 1000 + ((time).tv_usec) / 1000) */ \
        (((time).tv_sec) * 1000 + ((time).tv_usec) / CONFIG_TIME_DIVISOR)
        /* END:   Modified by Mabinjie at 20110517 */

/* ======================================
 *              IMPLEMENTATIONS
 * ====================================== */

/*
 *      Unaligned macros implementation
 */

#define UW_CONVERT_BE(func, ptr, val)  \
    uw_packed_##func(ptr, (val), 1, 0, 3, 2, 1, 0)

#define UW_CONVERT_LE(func, ptr, val)  \
    uw_packed_##func(ptr, (val), 0, 1, 0, 1, 2, 3)

#define UW_PGET(ptr, byte)        (((uint8_t *)(ptr))[byte])
#define UW_PSET(ptr, byte, val)    ((uint8_t *)(ptr))[byte] = (uint8_t)(val)

#define uw_packed_get16(ptr, val, a0, a1, b0, b1, b2, b3) \
    ((((uint16_t)UW_PGET(ptr, a0)) <<  0) | \
     (((uint16_t)UW_PGET(ptr, a1)) <<  8))

#define uw_packed_get32(ptr, val, a0, a1, b0, b1, b2, b3) \
    ((((uint32_t)UW_PGET(ptr, b0)) <<  0) | \
     (((uint32_t)UW_PGET(ptr, b1)) <<  8) | \
     (((uint32_t)UW_PGET(ptr, b2)) << 16) | \
     (((uint32_t)UW_PGET(ptr, b3)) << 24))

#define uw_packed_set16(ptr, val, a0, a1, b0, b1, b2, b3) \
    do { \
        UW_PSET(ptr, a0, (val) >>  0); \
        UW_PSET(ptr, a1, (val) >>  8); \
    } while (0)

#define uw_packed_set32(ptr, val, a0, a1, b0, b1, b2, b3) \
    do { \
        UW_PSET(ptr, b0, (val) >>  0); \
        UW_PSET(ptr, b1, (val) >>  8); \
        UW_PSET(ptr, b2, (val) >> 16); \
        UW_PSET(ptr, b3, (val) >> 24); \
    } while (0)

/*
 *      Endianess handling macros implementation
 */
#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
# define UW_SWAP_BE     UW_NO_SWAP
# define UW_SWAP_LE     UW_SWAP

#elif CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN
# define UW_SWAP_BE     UW_SWAP
# define UW_SWAP_LE     UW_NO_SWAP

#else
# error "Unknown byte order"
#endif

#define uw_le16(val)               UW_SWAP_LE(((uint16_t)val), 16)
#define uw_le32(val)               UW_SWAP_LE(((uint32_t)val), 32)
#define uw_be16(val)               UW_SWAP_BE(((uint16_t)val), 16)
#define uw_be32(val)               UW_SWAP_BE(((uint32_t)val), 32)

#define UW_SWAP(val, bits)         uw_bswap##bits(val)
#define UW_NO_SWAP(val, bits)      (val)

#define uw_bswap16(val) \
        (((val) >> 8) | (((val) << 8) & 0xff00))
#define uw_bswap32(val) \
        (((val) >> 24) | (((val) >> 8) & 0x0000ff00) | \
        (((val) << 8) & 0x00ff0000) | ((val) << 24))

/*
 *      Backwards compatibility macros
 */

#define UGETW(ptr)              ua_get_le16(ptr)
#define UGETDW(ptr)             ua_get_le32(ptr)
#define USETW(ptr,v)            ua_set_le16(ptr, v)
#define USETDW(ptr,v)           ua_set_le32(ptr, v)

#define USETW2(w,h,l)           ((w)[0] = (uint8_t)(l), (w)[1] = (uint8_t)(h))

#define ua_be16toh(ptr)         ua_get_be16(ptr)
#define ua_be32toh(ptr)         ua_get_be32(ptr)

#define ua_htobe16(ptr,v)       ua_set_be16(ptr, v)
#define ua_htobe32(ptr,v)       ua_set_be32(ptr, v)

#define ua_le16toh(ptr)         ua_get_le16(ptr)
#define ua_le32toh(ptr)         ua_get_le32(ptr)

#define ua_htole16(ptr,v)       ua_set_le16(ptr, v)
#define ua_htole32(ptr,v)       ua_set_le32(ptr, v)

#define le16toh(x)              letoh16(x)
#define le32toh(x)              letoh32(x)

#define be16toh(x)              betoh16(x)
#define be32toh(x)              betoh32(x)

#define htobe16_t(ptr, x)       ua_set_be16(ptr, x)
#define htobe32_t(ptr, x)       ua_set_be32(ptr, x)

#define htole16_t(ptr, x)       ua_set_le16(ptr, x)
#define htole32_t(ptr, x)       ua_set_le32(ptr, x)

#define be16toh_t(ptr)          ua_get_be16(ptr)
#define be32toh_t(ptr)          ua_get_be32(ptr)

#define bswap16                 uw_bswap16
#define bswap32                 uw_bswap32

#define letoh16(val)            uw_letoh16(val)
#define letoh32(val)            uw_letoh32(val)
#define htole16(val)            uw_htole16(val)
#define htole32(val)            uw_htole32(val)

#define betoh16(val)            uw_betoh16(val)
#define betoh32(val)            uw_betoh32(val)
#define htobe16(val)            uw_htobe16(val)
#define htobe32(val)            uw_htobe32(val)
#endif

typedef u8 uw_u16_t[2];

/* Global mandatory definitions */
#define __RESTRICT__
#define __VOLATILE__ volatile
#define __INLINE__ __inline__
#define __PACKED_PRE__
#define __PACKED_POST__ __attribute__ ((packed))

/*#define ua_be16toh(x)       __be16_to_cpu(x)*/
/*#define ua_htobe16(ptr,v)   *(u16 *)ptr = __cpu_to_be16(v)*/

#define UW_MAC_ADDR_LEN         6
#define IP_ADDR_LEN             4
#define ETHERNET_MAX_FRAME_SIZE 1514 /* without Ethernet header */
#define LOOPBACK_SEND_DATA_LEN 4096
#define PROTO_IP                0x0800
#define PROTO_IPV6              0x86dd
#define PROTO_ARP               0x0806

#define PROTO_IP_ICMP           0x01
#define PROTO_IP_UDP            0x11

#define ARP_OPCODE_REQUEST      0x0001
#define ARP_OPCODE_REPLY        0x0002

#define ICMP_TYPE_PING_REQUEST  0x08
#define ICMP_TYPE_PING_REPLY    0x00

#define IP_VERSION(ip) (((ip)->hdr.version_and_header_length & 0xF0) >> 4)
#define IP_HEADER_LENGTH(ip) ((ip)->hdr.version_and_header_length & 0x0F)

typedef enum {
    NET_DEV_ETH = 0,
    NET_DEV_WIFI
} net_dev_type_t;

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif // WIN32
typedef __PACKED_PRE__ struct {
    u8  type;
    u8  code;
    u16 chk;
    u16 ident;
    u16 seq;
} __PACKED_POST__ icmp_header_t;

typedef __PACKED_PRE__ struct {
    icmp_header_t   hdr;
    u8              data[1];
} __PACKED_POST__ icmp_t;

typedef __PACKED_PRE__ struct {
    u16 src_port;
    u16 dst_port;
    u16 length;
    u16 chk;
} __PACKED_POST__ udp_header_t;

typedef __PACKED_PRE__ struct {
    udp_header_t    hdr;
    u8              data[1];
} __PACKED_POST__ udp_t;

typedef __PACKED_PRE__ struct {
    u8     version_and_header_length;
    u8     dsf;
    u16    length;
    u16    ident;
    u16    flags_frag_offset;
    u8     ttl;
    u8     proto;
    u16    chk;
    u8     sa[IP_ADDR_LEN];
    u8     da[IP_ADDR_LEN];
} __PACKED_POST__ ip_header_t;

typedef __PACKED_PRE__ union {
    u8 raw_data[1];
    icmp_t  icmp;
    udp_t   udp;
} __PACKED_POST__ ip_data_t;

typedef __PACKED_PRE__ struct {
    ip_header_t hdr;
    ip_data_t   data;
} __PACKED_POST__ ip_t;

typedef __PACKED_PRE__ struct {
    u16    hw_type;
    u16    pr_type;
    u8     hw_size;
    u8     pr_size;
    u16    opcode;
    u8     sma[UW_MAC_ADDR_LEN];
    u8     sia[IP_ADDR_LEN];
    u8     dma[UW_MAC_ADDR_LEN];
    u8     dia[IP_ADDR_LEN];
    u8     padding[18];
} __PACKED_POST__ arp_t;

typedef __PACKED_PRE__ union {
    u8 raw_data[1];
    arp_t   arp;
    ip_t    ip;
} __PACKED_POST__ eth_data_t;

typedef __PACKED_PRE__ struct {
    u8     da[UW_MAC_ADDR_LEN];
    u8     sa[UW_MAC_ADDR_LEN];
    uw_u16_t    type;
} __PACKED_POST__ eth_header_t;

typedef __PACKED_PRE__ struct {
    eth_header_t  hdr;
    eth_data_t    packet;
} __PACKED_POST__ eth_frame_t;

#ifdef WIN32
#pragma pack(pop)
#endif // WIN32
typedef union
{
    u8         raw_data[ETHERNET_MAX_FRAME_SIZE];
    eth_frame_t     frame;
} net_buffer_t;

#endif /* __NCM_BALONG_TEST_H__ */
