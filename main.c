#include <stdio.h>
#include <errno.h>
#include "popen_plus.h"

extern const char* __progname;

/* tab+name+blank */
int max_assign_len = 0;

int old_stdout_fd;

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
} higig2_t;

struct person
{
    char*    name;
    uint64_t age;
    int      height;
    higig2_t h2;
};

enum
{
    S_IDLE,
    S_GET_VALUE,
    S_GET_NAME,
};

void print_blanks(int num)
{
    while (num--)
    {
        printf(" ");
    }
}

void print_name(char* name, int level)
{
    int len = level * 4;
    print_blanks(len);
    printf(".%s ", name);
    len += strlen(name) + 2;

    if (len > max_assign_len)
    {
        max_assign_len = len;
    }

    print_blanks(max_assign_len - len);

    printf("= ");
}

int get_token(FILE* fp, char* token)
{
    int   ret = -1; /* no token */
    int   c;
    char* p_char  = token;
    int   is_done = 0;
    while (!is_done)
    {
        c = fgetc(fp);

        switch (c)
        {
        case ' ':
        case '\r':
        case '\n':
        case '\t':
        case ',':
            if (ret != -1)
            {
                is_done = 1;
            }
            break;

        case EOF:
            is_done = 1;
            break;

        case '{':
        case '}':
        case '=':
            if (ret == -1)
            {
                return c;
            }
            ungetc(c, fp);
            is_done = 1;
            break;

        default:
            *p_char++ = c;
            ret       = 1;
        }
    }

    *p_char = '\0';

    return ret;
}

int get_type_name(FILE* fp, char* type_name)
{
    int         ret  = -1;
    char*       line = NULL, * p;
    size_t      len;
    const char* match_key = "type = ";

    int is_done = 0;
    while (!is_done)
    {
        if (getline(&line, &len, fp) < 0)
        {
            break;
        }

        if ((p = strstr(line, match_key)) != NULL)
        {
            ret     = 1;
            is_done = 1;
            p      += strlen(match_key);
            strcpy(type_name, p);
            type_name[strlen(type_name) - 1] = '\0';
        }
    }

    free(line);

    return ret;
}

void stat_parsor(FILE* fp)
{
    int  state       = S_IDLE;
    int  brace_level = 0;
    int  token_ret;
    char token[256];

    int is_done = 0;
    while (!is_done)
    {
        token_ret = get_token(fp, token);

        if (token_ret == -1)
        {
            break;
        }

        switch (state)
        {
        case S_IDLE:
            if (token_ret == '=')
            {
                state = S_GET_VALUE;
            }
            break;

        case S_GET_VALUE:
            if (token_ret == '{')
            {
                brace_level++;
                state = S_GET_NAME;
                printf("\n");
                print_blanks(4 * (brace_level - 1));
                printf("{\n");
            }
            else
            {
                /* print the value */
                printf("%s,\n", token);
                state = S_GET_NAME;
            }
            break;

        case S_GET_NAME:
            if (token_ret == '}')
            {
                print_blanks(4 * (brace_level - 1));
                printf("}\n");
                brace_level--;
                if (brace_level == 0)
                {
                    is_done = 1;
                }
            }
            else
            {
                print_name(token, brace_level);
                state = S_IDLE;
            }
            break;
        }
    }
}

#define DUMP_STRUCT(pointer) dump_struct(# pointer, pointer)

void dump_struct(const char* name, void* address)
{
    struct popen_plus_process* p_fp;

    char buf[256];
    int  flag = 1;

    sprintf(buf, "gdb -n -q %s %d", __progname, getpid());
    p_fp = popen_plus(buf);

    fprintf(p_fp->write_fp, "thread 1\n");
    fprintf(p_fp->write_fp, "set var flag=0\n");
    fprintf(p_fp->write_fp, "frame 1\n");
    fprintf(p_fp->write_fp, "whatis %s\n", name);
    fprintf(p_fp->write_fp, "p/x *%s\n", name);
    fprintf(p_fp->write_fp, "p/x *%s\n", name);
    fprintf(p_fp->write_fp, "detach\nquit\n");
    fflush(p_fp->write_fp);

    while (flag)  /* trap here */
    {
    }
    if (get_type_name(p_fp->read_fp, buf) < 0)
    {
        printf("can't get type name");
    }
    else
    {
        printf("(%s)(%s) = /* @%p */", buf, name, address);
    }

    max_assign_len = 0;
    old_stdout_fd  = dup(fileno(stdout));
    freopen("/dev/null", "w", stdout);

    stat_parsor(p_fp->read_fp);
    fflush(stdout);

    dup2(old_stdout_fd, fileno(stdout));
    close(old_stdout_fd);

    stat_parsor(p_fp->read_fp);

    popen_plus_close(p_fp);
}


int main()
{
    struct person johndoe;

    typedef struct person person_t;
    person_t* p = &johndoe;
    johndoe.age = 10;

    DUMP_STRUCT(p);

    return 0;
}

