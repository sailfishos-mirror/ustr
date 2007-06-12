/* Copyright (c) 2007 James Antill -- See LICENSE file for terms. */

#ifndef USTR_UTF8_H
#error " You should have already included ustr-utf8.h, or just include ustr.h."
#endif

/* import and hacked from:
   http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c 2007-06-04 */
/*
 * This is an implementation of wcwidth() and wcswidth() (defined in
 * IEEE Std 1002.1-2001) for Unicode.
 *
 * http://www.opengroup.org/onlinepubs/007904975/functions/wcwidth.html
 * http://www.opengroup.org/onlinepubs/007904975/functions/wcswidth.html
 *
 * In fixed-width output devices, Latin characters all occupy a single
 * "cell" position of equal width, whereas ideographic CJK characters
 * occupy two such cells. Interoperability between terminal-line
 * applications and (teletype-style) character terminals using the
 * UTF-8 encoding requires agreement on which character should advance
 * the cursor by how many cell positions. No established formal
 * standards exist at present on which Unicode character shall occupy
 * how many cell positions on character terminals. These routines are
 * a first attempt of defining such behavior based on simple rules
 * applied to data provided by the Unicode Consortium.
 *
 * For some graphical characters, the Unicode standard explicitly
 * defines a character-cell width via the definition of the East Asian
 * FullWidth (F), Wide (W), Half-width (H), and Narrow (Na) classes.
 * In all these cases, there is no ambiguity about which width a
 * terminal shall use. For characters in the East Asian Ambiguous (A)
 * class, the width choice depends purely on a preference of backward
 * compatibility with either historic CJK or Western practice.
 * Choosing single-width for these characters is easy to justify as
 * the appropriate long-term solution, as the CJK practice of
 * displaying these characters as double-width comes from historic
 * implementation simplicity (8-bit encoded characters were displayed
 * single-width and 16-bit ones double-width, even for Greek,
 * Cyrillic, etc.) and not any typographic considerations.
 *
 * Much less clear is the choice of width for the Not East Asian
 * (Neutral) class. Existing practice does not dictate a width for any
 * of these characters. It would nevertheless make sense
 * typographically to allocate two character cells to characters such
 * as for instance EM SPACE or VOLUME INTEGRAL, which cannot be
 * represented adequately with a single-width glyph. The following
 * routines at present merely assign a single-cell width to all
 * neutral characters, in the interest of simplicity. This is not
 * entirely satisfactory and should be reconsidered before
 * establishing a formal standard in this area. At the moment, the
 * decision which Not East Asian (Neutral) characters should be
 * represented by double-width glyphs cannot yet be answered by
 * applying a simple rule from the Unicode database content. Setting
 * up a proper standard for the behavior of UTF-8 character terminals
 * will require a careful analysis not only of each Unicode character,
 * but also of each presentation form, something the author of these
 * routines has avoided to do so far.
 *
 * http://www.unicode.org/unicode/reports/tr11/
 *
 * Markus Kuhn -- 2007-05-26 (Unicode 5.0)
 *
 * Permission to use, copy, modify, and distribute this software
 * for any purpose and without fee is hereby granted. The author
 * disclaims all warranties with regard to this software.
 *
 * Latest version: http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
 */

/* auxiliary function for binary search in interval table */
USTR_CONF_i_PROTO
int ustr__utf8_bisearch(USTR__UTF8_WCHAR ucs,
                        const struct ustr__utf8_interval *table, int max)
{
  int min = 0;
  int mid;

  if (ucs < table[0].first || ucs > table[max].last)
    return (USTR_FALSE);
  while (max >= min) {
    mid = (min + max) / 2;
    if (ucs > table[mid].last)
      min = mid + 1;
    else if (ucs < table[mid].first)
      max = mid - 1;
    else
      return (USTR_TRUE);
  }

  return (USTR_FALSE);
}


/* The following two functions define the column width of an ISO 10646
 * character as follows:
 *
 *    - The null character (U+0000) has a column width of 0.
 *
 *    - Other C0/C1 control characters and DEL will lead to a return
 *      value of -1.
 *
 *    - Non-spacing and enclosing combining characters (general
 *      category code Mn or Me in the Unicode database) have a
 *      column width of 0.
 *
 *    - SOFT HYPHEN (U+00AD) has a column width of 1.
 *
 *    - Other format characters (general category code Cf in the Unicode
 *      database) and ZERO WIDTH SPACE (U+200B) have a column width of 0.
 *
 *    - Hangul Jamo medial vowels and final consonants (U+1160-U+11FF)
 *      have a column width of 0.
 *
 *    - Spacing characters in the East Asian Wide (W) or East Asian
 *      Full-width (F) category as defined in Unicode Technical
 *      Report #11 have a column width of 2.
 *
 *    - All remaining characters (including all printable
 *      ISO 8859-1 and WGL4 characters, Unicode control characters,
 *      etc.) have a column width of 1.
 *
 * This implementation assumes that wchar_t characters are encoded
 * in ISO 10646.
 */

USTR_CONF_i_PROTO
ssize_t ustr__utf8_mk_wcwidth(USTR__UTF8_WCHAR ucs)
{
  /* sorted list of non-overlapping intervals of non-spacing characters */
  /* generated by "uniset +cat=Me +cat=Mn +cat=Cf -00AD +1160-11FF +200B c" */
  static const struct ustr__utf8_interval combining[] = {
    { 0x0300, 0x036F }, { 0x0483, 0x0486 }, { 0x0488, 0x0489 },
    { 0x0591, 0x05BD }, { 0x05BF, 0x05BF }, { 0x05C1, 0x05C2 },
    { 0x05C4, 0x05C5 }, { 0x05C7, 0x05C7 }, { 0x0600, 0x0603 },
    { 0x0610, 0x0615 }, { 0x064B, 0x065E }, { 0x0670, 0x0670 },
    { 0x06D6, 0x06E4 }, { 0x06E7, 0x06E8 }, { 0x06EA, 0x06ED },
    { 0x070F, 0x070F }, { 0x0711, 0x0711 }, { 0x0730, 0x074A },
    { 0x07A6, 0x07B0 }, { 0x07EB, 0x07F3 }, { 0x0901, 0x0902 },
    { 0x093C, 0x093C }, { 0x0941, 0x0948 }, { 0x094D, 0x094D },
    { 0x0951, 0x0954 }, { 0x0962, 0x0963 }, { 0x0981, 0x0981 },
    { 0x09BC, 0x09BC }, { 0x09C1, 0x09C4 }, { 0x09CD, 0x09CD },
    { 0x09E2, 0x09E3 }, { 0x0A01, 0x0A02 }, { 0x0A3C, 0x0A3C },
    { 0x0A41, 0x0A42 }, { 0x0A47, 0x0A48 }, { 0x0A4B, 0x0A4D },
    { 0x0A70, 0x0A71 }, { 0x0A81, 0x0A82 }, { 0x0ABC, 0x0ABC },
    { 0x0AC1, 0x0AC5 }, { 0x0AC7, 0x0AC8 }, { 0x0ACD, 0x0ACD },
    { 0x0AE2, 0x0AE3 }, { 0x0B01, 0x0B01 }, { 0x0B3C, 0x0B3C },
    { 0x0B3F, 0x0B3F }, { 0x0B41, 0x0B43 }, { 0x0B4D, 0x0B4D },
    { 0x0B56, 0x0B56 }, { 0x0B82, 0x0B82 }, { 0x0BC0, 0x0BC0 },
    { 0x0BCD, 0x0BCD }, { 0x0C3E, 0x0C40 }, { 0x0C46, 0x0C48 },
    { 0x0C4A, 0x0C4D }, { 0x0C55, 0x0C56 }, { 0x0CBC, 0x0CBC },
    { 0x0CBF, 0x0CBF }, { 0x0CC6, 0x0CC6 }, { 0x0CCC, 0x0CCD },
    { 0x0CE2, 0x0CE3 }, { 0x0D41, 0x0D43 }, { 0x0D4D, 0x0D4D },
    { 0x0DCA, 0x0DCA }, { 0x0DD2, 0x0DD4 }, { 0x0DD6, 0x0DD6 },
    { 0x0E31, 0x0E31 }, { 0x0E34, 0x0E3A }, { 0x0E47, 0x0E4E },
    { 0x0EB1, 0x0EB1 }, { 0x0EB4, 0x0EB9 }, { 0x0EBB, 0x0EBC },
    { 0x0EC8, 0x0ECD }, { 0x0F18, 0x0F19 }, { 0x0F35, 0x0F35 },
    { 0x0F37, 0x0F37 }, { 0x0F39, 0x0F39 }, { 0x0F71, 0x0F7E },
    { 0x0F80, 0x0F84 }, { 0x0F86, 0x0F87 }, { 0x0F90, 0x0F97 },
    { 0x0F99, 0x0FBC }, { 0x0FC6, 0x0FC6 }, { 0x102D, 0x1030 },
    { 0x1032, 0x1032 }, { 0x1036, 0x1037 }, { 0x1039, 0x1039 },
    { 0x1058, 0x1059 }, { 0x1160, 0x11FF }, { 0x135F, 0x135F },
    { 0x1712, 0x1714 }, { 0x1732, 0x1734 }, { 0x1752, 0x1753 },
    { 0x1772, 0x1773 }, { 0x17B4, 0x17B5 }, { 0x17B7, 0x17BD },
    { 0x17C6, 0x17C6 }, { 0x17C9, 0x17D3 }, { 0x17DD, 0x17DD },
    { 0x180B, 0x180D }, { 0x18A9, 0x18A9 }, { 0x1920, 0x1922 },
    { 0x1927, 0x1928 }, { 0x1932, 0x1932 }, { 0x1939, 0x193B },
    { 0x1A17, 0x1A18 }, { 0x1B00, 0x1B03 }, { 0x1B34, 0x1B34 },
    { 0x1B36, 0x1B3A }, { 0x1B3C, 0x1B3C }, { 0x1B42, 0x1B42 },
    { 0x1B6B, 0x1B73 }, { 0x1DC0, 0x1DCA }, { 0x1DFE, 0x1DFF },
    { 0x200B, 0x200F }, { 0x202A, 0x202E }, { 0x2060, 0x2063 },
    { 0x206A, 0x206F }, { 0x20D0, 0x20EF }, { 0x302A, 0x302F },
    { 0x3099, 0x309A }, { 0xA806, 0xA806 }, { 0xA80B, 0xA80B },
    { 0xA825, 0xA826 }, { 0xFB1E, 0xFB1E }, { 0xFE00, 0xFE0F },
    { 0xFE20, 0xFE23 }, { 0xFEFF, 0xFEFF }, { 0xFFF9, 0xFFFB },
    { 0x10A01, 0x10A03 }, { 0x10A05, 0x10A06 }, { 0x10A0C, 0x10A0F },
    { 0x10A38, 0x10A3A }, { 0x10A3F, 0x10A3F }, { 0x1D167, 0x1D169 },
    { 0x1D173, 0x1D182 }, { 0x1D185, 0x1D18B }, { 0x1D1AA, 0x1D1AD },
    { 0x1D242, 0x1D244 }, { 0xE0001, 0xE0001 }, { 0xE0020, 0xE007F },
    { 0xE0100, 0xE01EF }
  };

  /* test for 8-bit control characters */
  if (ucs == 0)
    return 0;

  if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0))
    return (-1);

  /* binary search in table of non-spacing characters */
  if (ustr__utf8_bisearch(ucs, combining, sizeof(combining) /
                          sizeof(struct ustr__utf8_interval) - 1))
    return 0;

  /* if we arrive here, ucs is not a combining or C0/C1 control character */

  return 1 + 
    (ucs >= 0x1100 &&
     (ucs <= 0x115f ||                    /* Hangul Jamo init. consonants */
      ucs == 0x2329 || ucs == 0x232a ||
      (ucs >= 0x2e80 && ucs <= 0xa4cf &&
       ucs != 0x303f) ||                  /* CJK ... Yi */
      (ucs >= 0xac00 && ucs <= 0xd7a3) || /* Hangul Syllables */
      (ucs >= 0xf900 && ucs <= 0xfaff) || /* CJK Compatibility Ideographs */
      (ucs >= 0xfe10 && ucs <= 0xfe19) || /* Vertical forms */
      (ucs >= 0xfe30 && ucs <= 0xfe6f) || /* CJK Compatibility Forms */
      (ucs >= 0xff00 && ucs <= 0xff60) || /* Fullwidth Forms */
      (ucs >= 0xffe0 && ucs <= 0xffe6) ||
      (ucs >= 0x20000 && ucs <= 0x2fffd) ||
      (ucs >= 0x30000 && ucs <= 0x3fffd)));
}

/* import and hacked from:
   http://www.cl.cam.ac.uk/~mgk25/ucs/utf8_check.c 2007-06-04 */
/*
 * The utf8_check() function scans the '\0'-terminated string starting
 * at s. It returns a pointer to the first byte of the first malformed
 * or overlong UTF-8 sequence found, or NULL if the string contains
 * only correct UTF-8. It also spots UTF-8 sequences that could cause
 * trouble if converted to UTF-16, namely surrogate characters
 * (U+D800..U+DFFF) and non-Unicode positions (U+FFFE..U+FFFF). This
 * routine is very likely to find a malformed sequence if the input
 * uses any other encoding than UTF-8. It therefore can be used as a
 * very effective heuristic for distinguishing between UTF-8 and other
 * encodings.
 *
 * Markus Kuhn <http://www.cl.cam.ac.uk/~mgk25/> -- 2005-03-30
 */
USTR_CONF_i_PROTO
USTR__UTF8_WCHAR ustr__utf8_check(const unsigned char **ps)
{
  const unsigned char *s = *ps;
  USTR__UTF8_WCHAR ret = 0;
  
  if (*s < 0x80)                  /* 0xxxxxxx */
  { 
    ret = *s;
    s++;
  }
  else if ((s[0] & 0xe0) == 0xc0) /* 110XXXXx 10xxxxxx */
  {
    if (((s[1] & 0xc0) != 0x80) ||
        ((s[0] & 0xfe) == 0xc0))                          /* overlong? */
      goto utf8_fail;
    
    ret = ((s[0] & 0x1f) << 6) | (s[1] & 0x3f);
    s += 2;
  }
  else if ((s[0] & 0xf0) == 0xe0) /* 1110XXXX 10Xxxxxx 10xxxxxx */
  {
    if (((s[1] & 0xc0) != 0x80) || ((s[2] & 0xc0) != 0x80) ||
        ((s[0] == 0xe0) && ((s[1] & 0xe0) == 0x80)) ||    /* overlong? */
        ((s[0] == 0xed) && ((s[1] & 0xe0) == 0xa0)) ||    /* surrogate? */
        ((s[0] == 0xef) &&  (s[1] == 0xbf) &&
         ((s[2] & 0xfe) == 0xbe))) /* U+FFFE or U+FFFF? */
      goto utf8_fail;
    
    ret = (((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f));
    s += 3;
  }
  else if ((s[0] & 0xf8) == 0xf0) /* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
  {
    if (((s[1] & 0xc0) != 0x80) ||
        ((s[2] & 0xc0) != 0x80) ||
        ((s[3] & 0xc0) != 0x80) ||
        ((s[0] == 0xf0) && ((s[1] & 0xf0) == 0x80)) ||    /* overlong? */
        ((s[0] == 0xf4) && (s[1] > 0x8f)) || (s[0] > 0xf4)) /* > U+10FFFF? */
      goto utf8_fail;
    
    ret = (((s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) |
           ((s[2] & 0x3f) <<  6) |  (s[3] & 0x3f));
    s += 4;
  }
  else
    goto utf8_fail;
  
  *ps = s;    
  return (ret);
  
 utf8_fail:
  *ps = NULL;
  return (0);
}

/* See: http://en.wikipedia.org/wiki/UTF-8#Description */
USTR_CONF_e_PROTO
const unsigned char *ustr__utf8_prev(const unsigned char *ptr, size_t len)
{ /* find the begining of the previous UTF-8 character, no checking */
  while (len--)
  {
    if ((*--ptr & 0xc0) != 0x80)
      return (ptr);
  }
  
  return (0);
}

USTR_CONF_e_PROTO
const unsigned char *ustr__utf8_next(const unsigned char *ptr)
{ /* find the begining of the next UTF-8 character, no checking.
   *   -- assumes NIL termination, so no length. */
  while (1)
  {
    if ((*++ptr & 0xc0) != 0x80)
      break;
  }
  
  return (ptr);
}

USTR_CONF_I_PROTO
int ustr_utf8_valid(const struct Ustr *s1)
{
  const unsigned char *beg  = (const unsigned char *)ustr_cstr(s1);
  const unsigned char *scan = beg;
  size_t ret = 0;

  USTR_ASSERT(ustr_assert_valid(s1));

  while (*scan)
  {
    USTR_ASSERT(ustr_len(s1) > (size_t)(scan - beg));
    
    ustr__utf8_check(&scan);
    if (!scan)
      return (USTR_FALSE);
    ++ret;
  }
  USTR_ASSERT(ustr_len(s1) >= (size_t)(scan - beg));
  
  if (ustr_len(s1) != (size_t)(scan - beg))
    return (USTR_FALSE); /* string contains a NIL byte */

  return (USTR_TRUE);
}

USTR_CONF_I_PROTO
size_t ustr_utf8_len(const struct Ustr *s1)
{ /* this is a "fast" version */
  const unsigned char *scan = (const unsigned char *)ustr_cstr(s1);
  size_t ret = 0;

  USTR_ASSERT(ustr_assert_valid(s1));

  while (*scan)
    ret += ((*scan++ & 0xc0) != 0x80);

  return (ret);
}

ssize_t ustr_utf8_width(const struct Ustr *s1)
{
  const unsigned char *beg  = (const unsigned char *)ustr_cstr(s1);
  const unsigned char *scan = beg;
  ssize_t ret = 0;
  
  USTR_ASSERT(ustr_assert_valid(s1));

  while (*scan)
  {
    USTR__UTF8_WCHAR tmp = 0;

    USTR_ASSERT(ustr_len(s1) > (size_t)(scan - beg));
    
    tmp = ustr__utf8_check(&scan);
    if (!scan)
      return (0);
    ret += ustr__utf8_mk_wcwidth(tmp);    
  }
  USTR_ASSERT(ustr_len(s1) >= (size_t)(scan - beg));
  
  if (ustr_len(s1) != (size_t)(scan - beg))
    return (0); /* string contains a NIL byte */

  return (ret);
}

USTR_CONF_I_PROTO
size_t ustr_utf8_chars2bytes(const struct Ustr *s1, size_t pos, size_t len,
                             size_t *pret_pos)
{
  const unsigned char *beg  = (const unsigned char *)ustr_cstr(s1);
  const unsigned char *scan = beg;
  const unsigned char *ret_beg = beg;
  size_t ret_pos = 0;

  USTR_ASSERT(ustr__valid_subustr(s1, pos, len) || !len);
  USTR_ASSERT(pret_pos || (pos == 1));

  while (*scan)
  {
    const unsigned char *prev = scan;
    
    USTR_ASSERT(ustr_len(s1) > (size_t)(scan - beg));

    scan = ustr__utf8_next(scan);
    if (!--pos)
    {
      ret_beg = prev;
      ret_pos = (ret_beg - beg) + 1;
      break;
    }
  }

  if (len)
    while (*scan && --len)
    {
      USTR_ASSERT(ustr_len(s1) > (size_t)(scan - beg));
      
      scan = ustr__utf8_next(scan);
    }
  
  USTR_ASSERT(ustr_len(s1) >= (size_t)(scan - beg));
  
  if (len > 1)
    return (0); /* string contains a NIL byte, or ends with a bad UTF-8 char */

  if (pret_pos)
    *pret_pos = ret_pos;
  
  return (scan - ret_beg);
}

USTR_CONF_I_PROTO
size_t ustr_utf8_bytes2chars(const struct Ustr *s1, size_t pos, size_t len,
                             size_t *pret_pos)
{
  const unsigned char *beg  = (const unsigned char *)ustr_cstr(s1);
  const unsigned char *scan = beg;
  const unsigned char *ret_beg = beg;
  size_t clen = ustr__valid_subustr(s1, pos, len);
  size_t unum = 0;
  size_t ret_pos = 0;

  USTR_ASSERT(pret_pos || (pos == 1));

  if (!clen) /* bad position */
    return (0);
  
  scan += pos;
  if (!(ret_beg = ustr__utf8_prev(scan, pos)))
    return (0);
  USTR_ASSERT(ustr_len(s1) >= (size_t)(scan - beg));  

  scan = beg;
  while (scan < ret_beg)
    unum += ((*scan++ & 0xc0) != 0x80);
  
  unum += ((*scan & 0xc0) != 0x80);
  ret_pos = unum;

  if (len)
  {
    ret_beg += len - 1;
    USTR_ASSERT(ustr_len(s1) >= (size_t)(ret_beg - beg));
    
    while (scan <= ret_beg)
      unum += ((*scan++ & 0xc0) != 0x80);
  }
  
  if (pret_pos)
    *pret_pos = ret_pos;
  
  return (unum - ret_pos);
}
