/* Copyright (c) 2007 Paul Rosenfeld
                      James Antill -- See LICENSE file for terms. */
#ifndef USTR_SUB_H
#define USTR_SUB_H 1

#ifndef USTR_MAIN_H
# error " You should include ustr-main.h before this file, or just ustr.h"
#endif

USTR_CONF_E_PROTO int ustr_sub_buf(struct Ustr **, size_t, const void *, size_t)
    USTR__COMPILE_ATTR_NONNULL_A();
USTR_CONF_E_PROTO int ustr_sub(struct Ustr **, size_t, const struct Ustr *)
    USTR__COMPILE_ATTR_NONNULL_A();
USTR_CONF_EI_PROTO int ustr_sub_cstr(struct Ustr **, size_t, const char *)
    USTR__COMPILE_ATTR_NONNULL_A();
USTR_CONF_E_PROTO
int ustr_sub_subustr(struct Ustr **, size_t, const struct Ustr *, size_t,size_t)
    USTR__COMPILE_ATTR_NONNULL_A();
/* FIXME: ustr_sub_rep_chr() */

/* FIXME: ustrp_*() */

USTR_CONF_E_PROTO
int ustr_sc_sub_buf(struct Ustr **, size_t, size_t, const void *, size_t)
    USTR__COMPILE_ATTR_NONNULL_A();
USTR_CONF_E_PROTO
int ustr_sc_sub(struct Ustr **, size_t, size_t, const struct Ustr *)
    USTR__COMPILE_ATTR_NONNULL_A();
USTR_CONF_EI_PROTO
int ustr_sc_sub_cstr(struct Ustr **, size_t, size_t, const char *)
    USTR__COMPILE_ATTR_NONNULL_A();

/* FIXME: ustrp_*() */

USTR_CONF_E_PROTO size_t ustr_sc_replace(struct Ustr **, const struct Ustr *,
                                         const struct Ustr *, size_t)
    USTR__COMPILE_ATTR_NONNULL_A();

/* FIXME: ustrp_*() */

/*
#if USTR_CONF_INCLUDE_INTERNAL_HEADERS
# include "ustr-sub-internal.h"
#endif
*/

#if USTR_CONF_INCLUDE_CODEONLY_HEADERS
# include "ustr-sub-code.h"
#endif

#if USTR_CONF_COMPILE_USE_INLINE
USTR_CONF_II_PROTO int ustr_sub_cstr(struct Ustr **s1, size_t p, const char *c) 
{ return (ustr_sub_buf(s1, p, c, strlen(c))); }
USTR_CONF_II_PROTO
int ustr_sc_sub_cstr(struct Ustr **s1, size_t pos, size_t len,const char *cstr) 
{ return (ustr_sc_sub_buf(s1, pos, len, cstr, strlen(cstr))); }
#endif


#endif
