#include <stdio.h>
#include <inttypes.h>
#include "../gdb_print.h"

typedef struct _higig2               /* Byte # */
{                                    /* "Legacy" PPD Overlay 1 */
    uint32_t start              : 8; /* 0 */
    uint32_t tc                 : 4; /* 1 */
    uint32_t mcst               : 1;
    uint32_t _rsvd1             : 3;
    uint32_t dst_mod            : 8;             /* 2 */
    uint32_t dst_port           : 8;             /* 3 */
    uint32_t src_mod            : 8;             /* 4 */
    uint32_t src_port           : 8;             /* 5 */
    uint32_t lbid               : 8;             /* 6 */
    uint32_t ppd_type           : 3;             /* 7 */
    uint32_t _rsvd2             : 2;
    uint32_t ehv                : 1;
    uint32_t dp                 : 2;
    uint32_t mirror             : 1;               /* 8 */
    uint32_t mirror_done        : 1;
    uint32_t mirror_only        : 1;
    uint32_t ingress_tagged     : 1;
    uint32_t lag_failover       : 1;
    uint32_t donot_learn        : 1;
    uint32_t donot_modify       : 1;
    uint32_t dst_t              : 1;
    uint32_t vc_label_19_16     : 4;       /* 9 */
    uint32_t label_present      : 1;
    uint32_t l3                 : 1;
    uint32_t label_overlay_type : 2;
    uint32_t vc_label_15_8      : 8;        /* 10 */
    uint32_t vc_label_7_0       : 8;        /* 11 */
    uint32_t vlan_id_hi         : 4;        /* 12 */
    uint32_t vlan_cfi           : 1;
    uint32_t vlan_pri           : 3;
    uint32_t vlan_id_lo         : 8;           /* 13 */
    uint32_t opcode             : 3;           /* 14 */
    uint32_t preserve_dot1p     : 1;
    uint32_t preserve_dscp      : 1;
    uint32_t src_t              : 1;
    uint32_t pfm                : 2;
    uint32_t _rsvd5             : 5;               /* 15 */
    uint32_t hdr_ext_len        : 3;
    int      a[3][4];
} higig2_t;

typedef struct person_s
{
    char     xx[4];
    char*    name;
    uint64_t age;
    int      height;
    higig2_t h2;
} person_t;

int main()
{
    int             a[256] = { 1 };
    struct person_s johndoe;
    person_t*       p = &johndoe;

    johndoe.age = 10 + a[0];

    p = p;

    GDB_PRINT(p);
    GDB_PRINT(*p);
    GDB_PRINT(johndoe);
    GDB_PRINT(a);
    GDB_PRINT(a[1] + 10);
    GDB_PRINT(sizeof(johndoe));

    return 0;
}

