#ifndef LIBS_SBUF
#define LIBS_SBUF

#include <stdarg.h>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx) __attribute__((format (printf, fmt_idx, arg_idx)))
#else
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx)
#endif

#define _S(SB) string_raw((SB))

struct string;

struct string *string_new(size_t reserve);
void string_free(struct string *sb);

void string_clear(struct string *sb);
char *string_detach(struct string *sb);
void string_move(struct string *src, struct string *dest);
void string_dup(struct string *src, struct string *dest);

void string_putc(struct string *sb, const char c);

void string_cat(struct string *sb, const char *str);
void string_ncat(struct string *sb, const char *str, size_t len);
void string_bcat(struct string *sb, const struct string *buf);
void string_catf(struct string *sb, const char *fmt, ...) GNUC_PRINTF_CHECK(2,3);

int string_cmp(struct string *sb, const char *str);
int string_ncmp(struct string *sb, const char *str, size_t len);
int string_bcmp(struct string *sb, const struct string *buf);

const char *string_raw(struct string *sb);
size_t string_len(struct string *sb);

#endif
