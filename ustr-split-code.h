/* Copyright (c) 2007 Paul Rosenfeld
                      James Antill -- See LICENSE file for terms. */
#ifndef USTR_SPLIT_H
#error " Include ustr-split.h before this file."
#endif

#if !defined(USTR_FMT_INTERNAL_H) && !defined(USTR_IO_H)
#include <stdio.h>
#endif

USTR_CONF_i_PROTO
struct Ustr *ustrp__split_buf(struct Ustr_pool *p,
                              const struct Ustr *s1, size_t *poff, 
                              const void *sep, size_t slen, struct Ustr *ret,
                              unsigned int flags)
{
  size_t len = ustr_len(s1);
  size_t off = *poff;
  size_t found_pos = 0;
  size_t ret_len   = 0;
  
  USTR_ASSERT(ustr_assert_valid(s1));

  USTR_ASSERT_RET(off <= len, USTR_NULL);

  if (!slen || (off == len))
  {
    ustrp__free(p, ret);
    return (USTR_NULL);
  }
  
  /* Separator not found, just return the rest of the string */
  if (!(found_pos = ustr_srch_buf_fwd(s1, off, sep, slen)))
  {
    ret_len = len - off;
    *poff   = len;
    goto copy_buf;
  }

  /* Set the offset for the next, must skip sep */
  *poff = (found_pos - 1) + slen;
  
  /* If we don't wish to return blanks or separators, we might get "a,,b" with
   * sep="," so we need to skip the first "found" separator -- so just try
   * again */
  if (((found_pos - 1) == off) &&
      !(flags & (USTR_FLAG_SPLIT_RET_SEP | USTR_FLAG_SPLIT_RET_NON)))
    return (ustrp__split_buf(p, s1, poff, sep, slen, ret, flags));

  ret_len = (found_pos - 1) - off;
  if (flags & USTR_FLAG_SPLIT_RET_SEP) /* Include sep in the return value */
    ret_len += slen;
  
 copy_buf:
  if (ret)
  {
    if (!ustrp__set_subustr(p, &ret, s1, off + 1, ret_len))
      return (USTR_NULL);
    
    return (ret);
  }
  
  if (flags & USTR_FLAG_SPLIT_KEEP_CONF)
    return (ustrp__dup_subustr(p, s1, off + 1, ret_len));

  return (ustrp__dupx_buf(p, USTR__DUPX_DEF, ustr_cstr(s1) + off, ret_len));
}
USTR_CONF_I_PROTO
struct Ustr *ustr_split_buf(const struct Ustr *s1, size_t *off, 
                            const void *sep, size_t slen, struct Ustr *ret,
                            unsigned int flags)
{ return (ustrp__split_buf(0, s1, off, sep, slen, ret, flags)); }
USTR_CONF_I_PROTO
struct Ustrp *ustrp_split_buf(struct Ustr_pool *p,
                              const struct Ustrp *s1, size_t *off, 
                              const void *sep, size_t slen, struct Ustrp *ret,
                              unsigned int flags)
{ return (USTRP(ustrp__split_buf(p, &s1->s, off, sep, slen, &ret->s, flags))); }

USTR_CONF_I_PROTO
struct Ustr *ustr_split(const struct Ustr *s1, size_t *off, 
                        const struct Ustr *sep, struct Ustr *ret,
                        unsigned int flags)
{
  USTR_ASSERT(ustr_assert_valid(sep));
  return (ustrp__split_buf(0, s1,off,ustr_cstr(sep),ustr_len(sep), ret, flags));
}

USTR_CONF_I_PROTO
struct Ustrp *ustrp_split(struct Ustr_pool *p,
                          const struct Ustrp *s1, size_t *off, 
                          const struct Ustrp *sep, struct Ustrp *ret,
                          unsigned int flags)
{
  USTR_ASSERT(ustrp_assert_valid(sep));
  return (USTRP(ustrp__split_buf(p, &s1->s, off, ustrp_cstr(sep),ustrp_len(sep),
                                 &ret->s, flags)));
}

USTR_CONF_i_PROTO
struct Ustr *ustrp__split_chrs(struct Ustr_pool *p, struct Ustr *s1, 
            size_t *poff, const char *seps, struct Ustr *ret , int flags)
{
  size_t len = ustr_len(s1);
  size_t off = *poff;
  if (off >= len)
  {
    ustrp__free(p,ret);
    return USTR_NULL;
  }
  const char *beg = ustr_cstr(s1);
  const char *start_off = beg + off;
  size_t sep_len = strlen(seps);
  size_t ret_len = 0; 
  /* want the earliest match, so default to the end of the string */
  const char *first_found = beg + len;
  size_t sep_found=0;
  size_t i;
  for (i=0; i < sep_len; ++i)
  {
    const char *tmp=NULL;
    if ((tmp = memchr(start_off, seps[i], (size_t)(first_found-start_off))))
    {
      if (tmp < first_found)
      {
        first_found=tmp;
        sep_found=i;
      }
    }
  }
  /* from this point forward, this function is very similar to 
   * ustrp__split_buf() */
  if (first_found == (beg + len)) 
  {
  /* i.e., unchanged by the loop, so a sep was not found */
    ret_len = len - off;
    *poff = len;
    goto copy_buf;
  }
  *poff = (size_t)(first_found - beg) + 1;
  /* skip sep and blanks */
  if (((size_t)(first_found - beg) == off) && 
        !(flags & (USTR_FLAG_SPLIT_RET_SEP | USTR_FLAG_SPLIT_RET_NON)))
    return (ustrp__split_chrs(p, s1, poff, seps, ret, flags));

  ret_len = (size_t)(first_found - beg) - off;
  if (flags & USTR_FLAG_SPLIT_RET_SEP) /* Include sep in the return value */
    ret_len += 1; 

copy_buf:
  if (ret)
  {
    if (!ustrp__set_subustr(p, &ret, s1, off + 1, ret_len))
      return (USTR_NULL);
    
    return (ret);
  }
  
  if (flags & USTR_FLAG_SPLIT_KEEP_CONF)
    return (ustrp__dup_subustr(p, s1, off + 1, ret_len));

  return (ustrp__dupx_buf(p, USTR__DUPX_DEF, ustr_cstr(s1) + off, ret_len));
}

USTR_CONF_I_PROTO
struct Ustr *ustr_sc_split_chrs(struct Ustr *s1, size_t *poff, 
                              const char *seps, struct Ustr *ret , int flags)
{  return ustrp__split_chrs(0, s1, poff, seps, ret, flags); }
