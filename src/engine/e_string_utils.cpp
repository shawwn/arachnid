//========================================================================
//	file:		e_string_utils.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_string_utils.h"
#include <ctype.h>
//========================================================================

//========================================================================
// Declarations
//========================================================================
// constants used in UTF-8 conversions
static const unsigned char UTF_LEAD1 = 0xC0;      //110xxxxx
static const unsigned char UTF_LEAD1_MASK = 0x1F; //00011111
static const unsigned char UTF_LEAD2 = 0xE0;      //1110xxxx
static const unsigned char UTF_LEAD2_MASK = 0x0F; //00001111
static const unsigned char UTF_LEAD3 = 0xF0;      //11110xxx
static const unsigned char UTF_LEAD3_MASK = 0x07; //00000111
static const unsigned char UTF_LEAD4 = 0xF8;      //111110xx
static const unsigned char UTF_LEAD4_MASK = 0x03; //00000011
static const unsigned char UTF_LEAD5 = 0xFC;      //1111110x
static const unsigned char UTF_LEAD5_MASK = 0x01; //00000001
static const unsigned char UTF_CONT = 0x80;       //10xxxxxx
static const unsigned char UTF_CONT_MASK = 0x3F;  //00111111
//========================================================================

//========================================================================
// Third-Party Code
//========================================================================

#define NUL			'\0'

/* Lookup table to quickly get the length in bytes of a UTF-8 character from
 * the first byte of a UTF-8 string.  Bytes which are illegal when used as the
 * first byte have a one, because these will be used separately. */
static char utf8len_tab[256] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /*bogus*/
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /*bogus*/
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1,
};


/*
 * Convert a UTF-8 byte sequence to a wide character.
 * If the sequence is illegal or truncated by a NUL the first byte is
 * returned.
 * Does not include composing characters, of course.
 */
int utf_ptr2char(const char* p)
{
    int         len;

    if (p[0] < 0x80)    /* be quick for ASCII */
        return p[0];

    len = utf8len_tab[p[0]];
    if (len > 1 && (p[1] & 0xc0) == 0x80)
    {
        if (len == 2)
            return ((p[0] & 0x1f) << 6) + (p[1] & 0x3f);
        if ((p[2] & 0xc0) == 0x80)
        {
            if (len == 3)
                return ((p[0] & 0x0f) << 12) + ((p[1] & 0x3f) << 6)
                    + (p[2] & 0x3f);
            if ((p[3] & 0xc0) == 0x80)
            {
                if (len == 4)
                    return ((p[0] & 0x07) << 18) + ((p[1] & 0x3f) << 12)
                        + ((p[2] & 0x3f) << 6) + (p[3] & 0x3f);
                if ((p[4] & 0xc0) == 0x80)
                {
                    if (len == 5)
                        return ((p[0] & 0x03) << 24) + ((p[1] & 0x3f) << 18)
                            + ((p[2] & 0x3f) << 12) + ((p[3] & 0x3f) << 6)
                            + (p[4] & 0x3f);
                    if ((p[5] & 0xc0) == 0x80 && len == 6)
                        return ((p[0] & 0x01) << 30) + ((p[1] & 0x3f) << 24)
                            + ((p[2] & 0x3f) << 18) + ((p[3] & 0x3f) << 12)
                            + ((p[4] & 0x3f) << 6) + (p[5] & 0x3f);
                }
            }
        }
    }
    /* Illegal value, just return the first byte */
    return p[0];
}


/*
 * Get the length of UTF-8 byte sequence "p[size]".  Does not include any
 * following composing characters.
 * Returns 1 for "".
 * Returns 1 for an illegal byte sequence (also in incomplete byte seq.).
 * Returns number > "size" for an incomplete byte sequence.
 */
int utf_ptr2len_len(const char* p, int size)
{
    int         len;
    int         i;
    int         m;

    if (*p == NUL)
        return 1;
    m = len = utf8len_tab[*p];
    if (len > size)
        m = size;       /* incomplete byte sequence. */
    for (i = 1; i < m; ++i)
        if ((p[i] & 0xc0) != 0x80)
            return 1;
    return len;
}


/*
 * Return the number of bytes the UTF-8 encoding of character "c" takes.
 * This does not include composing characters.
 */
int utf_char2len(int c)
{
    if (c < 0x80)
        return 1;
    if (c < 0x800)
        return 2;
    if (c < 0x10000)
        return 3;
    if (c < 0x200000)
        return 4;
    if (c < 0x4000000)
        return 5;
    return 6;
}


/*
 * Convert Unicode character "c" to UTF-8 string in "buf[]".
 * Returns the number of bytes.
 * This does not include composing characters.
 */
int utf_char2bytes(int c, byte* buf)
{
    if (c < 0x80)               /* 7 bits */
    {
        buf[0] = c;
        return 1;
    }
    if (c < 0x800)              /* 11 bits */
    {
        buf[0] = 0xc0 + ((unsigned)c >> 6);
        buf[1] = 0x80 + (c & 0x3f);
        return 2;
    }
    if (c < 0x10000)            /* 16 bits */
    {
        buf[0] = 0xe0 + ((unsigned)c >> 12);
        buf[1] = 0x80 + (((unsigned)c >> 6) & 0x3f);
        buf[2] = 0x80 + (c & 0x3f);
        return 3;
    }
    if (c < 0x200000)           /* 21 bits */
    {
        buf[0] = 0xf0 + ((unsigned)c >> 18);
        buf[1] = 0x80 + (((unsigned)c >> 12) & 0x3f);
        buf[2] = 0x80 + (((unsigned)c >> 6) & 0x3f);
        buf[3] = 0x80 + (c & 0x3f);
        return 4;
    }
    if (c < 0x4000000)          /* 26 bits */
    {
        buf[0] = 0xf8 + ((unsigned)c >> 24);
        buf[1] = 0x80 + (((unsigned)c >> 18) & 0x3f);
        buf[2] = 0x80 + (((unsigned)c >> 12) & 0x3f);
        buf[3] = 0x80 + (((unsigned)c >> 6) & 0x3f);
        buf[4] = 0x80 + (c & 0x3f);
        return 5;
    }
                                /* 31 bits */
    buf[0] = 0xfc + ((unsigned)c >> 30);
    buf[1] = 0x80 + (((unsigned)c >> 24) & 0x3f);
    buf[2] = 0x80 + (((unsigned)c >> 18) & 0x3f);
    buf[3] = 0x80 + (((unsigned)c >> 12) & 0x3f);
    buf[4] = 0x80 + (((unsigned)c >> 6) & 0x3f);
    buf[5] = 0x80 + (c & 0x3f);
    return 6;
}
//========================================================================

//===================
// IsWhitespace
//===================
bool			StrIsWhitespace(char c)
{
	const char* p(WHITESPACE);
	char cmp;
	while (cmp = *p++)
	{
		if (c == cmp)
			return true;
	}

	return false;
}


//===================
// IsWhitespace
//===================
bool			StrIsWhitespace(wchar_t c)
{
	const wchar_t* p(_T(WHITESPACE));
	wchar_t cmp;
	while (cmp = *p++)
	{
		if (c == cmp)
			return true;
	}

	return false;
}


//===================
// StrTrimLeft
//===================
string			StrTrimLeft(const string& str)
{
	size_t pos(0);
	for (; pos < str.size(); ++pos)
	{
		if (!StrIsWhitespace(str[pos]))
			break;
	}
	return str.substr(pos);
}


//===================
// StrTrimLeft
//===================
wstring			StrTrimLeft(const wstring& str)
{
	size_t pos(0);
	for (; pos < str.size(); ++pos)
	{
		if (!StrIsWhitespace(str[pos]))
			break;
	}
	return str.substr(pos);
}


//===================
// StrTrimRight
//===================
string			StrTrimRight(const string& str)
{
	size_t pos(str.size());
	for (; pos > 0; --pos)
	{
		if (!StrIsWhitespace(str[pos-1]))
			break;
	}
	return str.substr(0, pos);
}


//===================
// StrTrimRight
//===================
wstring			StrTrimRight(const wstring& str)
{
	size_t pos(str.size());
	for (; pos > 0; --pos)
	{
		if (!StrIsWhitespace(str[pos-1]))
			break;
	}
	return str.substr(0, pos);
}


//===================
// StrTrim
//===================
string			StrTrim(const string& str)
{
	return StrTrimLeft(StrTrimRight(str));
}


//===================
// StrTrim
//===================
wstring			StrTrim(const wstring& str)
{
	return StrTrimLeft(StrTrimRight(str));
}


//===================
// StrBeginsWith
//===================
bool			StrBeginsWith(const string& str, const string& with)
{
	if (StrEqualNum(str, with, with.size()))
		return true;
	return false;
}


//===================
// StrBeginsWith
//===================
bool			StrBeginsWith(const wstring& str, const wstring& with)
{
	if (StrEqualNum(str, with, with.size()))
		return true;
	return false;
}


//===================
// StrEndsWith
//===================
bool			StrEndsWith(const string& str, const string& with)
{
	if (str.size() < with.size())
		return false;
	if (StrEqualNum(str, with, with.size(), str.size() - with.size()))
		return true;
	return false;
}


//===================
// StrEndsWith
//===================
bool			StrEndsWith(const wstring& str, const wstring& with)
{
	if (str.size() < with.size())
		return false;
	if (StrEqualNum(str, with, with.size(), str.size() - with.size()))
		return true;
	return false;
}


//===================
// StrLowerString
//===================
string			StrLowerString(const string& str)
{
	size_t len(str.size());
	string result(len, ' ');
	for (size_t i = 0; i < len; ++i)
		result[i] = tolower(str[i]);
	return result;
}


//===================
// StrLowerString
//===================
wstring			StrLowerString(const wstring& str)
{
	size_t len(str.size());
	wstring result(len, _T(' '));
	for (size_t i = 0; i < len; ++i)
		result[i] = tolower(str[i]);
	return result;
}


//===================
// StrUpperString
//===================
string			StrUpperString(const string& str)
{
	size_t len(str.size());
	string result(len, ' ');
	for (size_t i = 0; i < len; ++i)
		result[i] = toupper(str[i]);
	return result;
}


//===================
// StrUpperString
//===================
wstring			StrUpperString(const wstring& str)
{
	size_t len(str.size());
	wstring result(len, _T(' '));
	for (size_t i = 0; i < len; ++i)
		result[i] = toupper(str[i]);
	return result;
}


//===================
// StrEqualNum
//===================
bool			StrEqualNum(const string& strA, const string& strB, size_t n, size_t off)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	size_t pos(off);
	while (n-- != 0)
	{
		if (pos >= sizeA || pos >= sizeB)
			return false;

		if (strA[pos] != strB[pos])
			return false;

		++pos;
	}

	return true;
}


//===================
// StrEqualNum
//===================
bool			StrEqualNum(const wstring& strA, const wstring& strB, size_t n, size_t off)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	size_t pos(off);
	while (n-- != 0)
	{
		if (pos >= sizeA || pos >= sizeB)
			return false;

		if (strA[pos] != strB[pos])
			return false;

		++pos;
	}

	return true;
}


//===================
// StrEqualNoCase
//===================
bool			StrEqualNoCase(const string& strA, const string& strB)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	if (sizeA != sizeB)
		return false;

	for (size_t pos(0); pos < sizeA; ++pos)
		if (tolower(strA[pos]) != tolower(strB[pos]))
			return false;

	return true;
}


//===================
// StrEqualNoCase
//===================
bool			StrEqualNoCase(const wstring& strA, const wstring& strB)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	if (sizeA != sizeB)
		return false;

	for (size_t pos(0); pos < sizeA; ++pos)
		if (tolower(strA[pos]) != tolower(strB[pos]))
			return false;

	return true;
}


//===================
// StrEqualNumNoCase
//===================
bool			StrEqualNumNoCase(const string& strA, const string& strB, size_t n, size_t off)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	size_t pos(off);
	while (n-- != 0)
	{
		if (pos >= sizeA || pos >= sizeB)
			return false;

		if (tolower(strA[pos]) != tolower(strB[pos]))
			return false;

		++pos;
	}

	return true;
}


//===================
// StrEqualNumNoCase
//===================
bool			StrEqualNumNoCase(const wstring& strA, const wstring& strB, size_t n, size_t off)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	size_t pos(off);
	while (n-- != 0)
	{
		if (pos >= sizeA || pos >= sizeB)
			return false;

		if (tolower(strA[pos]) != tolower(strB[pos]))
			return false;

		++pos;
	}

	return true;
}


//===================
// StrSplitBy
//===================
uint			StrSplitBy(strvec& results, const string& str, const string& delim)
{
	results.clear();

	size_t curPos(0);
	size_t findPos(str.find(delim));
	while (findPos != string::npos)
	{
		results.push_back(str.substr(curPos, findPos - curPos));
		curPos = findPos + delim.size();
		findPos = str.find(delim, curPos);
	}
	if (curPos < str.size())
		results.push_back(str.substr(curPos));

	return (uint)results.size();
}


//===================
// StrSplitBy
//===================
uint			StrSplitBy(wstrvec& results, const wstring& str, const wstring& delim)
{
	results.clear();

	size_t curPos(0);
	size_t findPos(str.find(delim));
	while (findPos != wstring::npos)
	{
		results.push_back(str.substr(curPos, findPos - curPos));
		curPos = findPos + delim.size();
		findPos = str.find(delim, curPos);
	}
	if (curPos < str.size())
		results.push_back(str.substr(curPos));

	return (uint)results.size();
}


//===================
// StrReplace
//===================
string			StrReplace(const string& str, const string& replacing, const string& replaceWith)
{
	string result;

	size_t curPos(0);
	size_t findPos(str.find(replacing));
	while (findPos != string::npos)
	{
		result.append(str.substr(curPos, findPos - curPos));
		result.append(replaceWith);
		curPos = findPos + replaceWith.size();
		findPos = str.find(replacing, curPos);
	}
	if (curPos < str.size())
		result.append(str.substr(curPos));
	return result;
}



//===================
// StrReplace
//===================
wstring			StrReplace(const wstring& str, const wstring& replacing, const wstring& replaceWith)
{
	wstring result;

	size_t curPos(0);
	size_t findPos(str.find(replacing));
	while (findPos != wstring::npos)
	{
		result.append(str.substr(curPos, findPos - curPos));
		result.append(replaceWith);
		curPos = findPos + replaceWith.size();
		findPos = str.find(replacing, curPos);
	}
	if (curPos < str.size())
		result.append(str.substr(curPos));
	return result;
}


//===================
// StrCompact
//===================
string			StrCompact(const string& str, const string& sequence, const string& replaceWith, size_t sequenceMinCount)
{
	string result;
	string remaining(str);
	size_t sequenceCount(0);
	while (!remaining.empty())
	{
		if (StrBeginsWith(remaining, sequence))
		{
			++sequenceCount;
			remaining = remaining.substr(sequence.size());
		}
		else
		{
			if (sequenceCount >= sequenceMinCount)
				result.append(replaceWith);
			else if (sequenceCount > 0)
			{
				do 
					result.append(sequence);
				while (--sequenceCount != 0);
			}
			sequenceCount = 0;

			size_t pos(remaining.find(sequence));
			if (pos == string::npos)
			{
				result.append(remaining);
				remaining.clear();
			}
			else
			{
				result.append(remaining.substr(0, pos));
				remaining = remaining.substr(pos);
			}
		}
	}

	if (sequenceCount >= sequenceMinCount)
		result.append(replaceWith);
	else if (sequenceCount > 0)
	{
		do 
		result.append(sequence);
		while (--sequenceCount != 0);
	}
	sequenceCount = 0;

	return result;
}


//===================
// StrCompact
//===================
wstring			StrCompact(const wstring& str, const wstring& sequence, const wstring& replaceWith, size_t sequenceMinCount)
{
	wstring result;
	wstring remaining(str);
	size_t sequenceCount(0);
	while (!remaining.empty())
	{
		if (StrBeginsWith(remaining, sequence))
		{
			++sequenceCount;
			remaining = remaining.substr(sequence.size());
		}
		else
		{
			if (sequenceCount >= sequenceMinCount)
				result.append(replaceWith);
			else if (sequenceCount > 0)
			{
				do 
				result.append(sequence);
				while (--sequenceCount != 0);
			}
			sequenceCount = 0;

			size_t pos(remaining.find(sequence));
			if (pos == wstring::npos)
			{
				result.append(remaining);
				remaining.clear();
			}
			else
			{
				result.append(remaining.substr(0, pos));
				remaining = remaining.substr(pos);
			}
		}
	}

	if (sequenceCount >= sequenceMinCount)
		result.append(replaceWith);
	else if (sequenceCount > 0)
	{
		do 
		result.append(sequence);
		while (--sequenceCount != 0);
	}
	sequenceCount = 0;

	return result;
}


//===================
// StrClassifyEncoding
//===================
ETextEncoding		StrClassifyEncoding(uint& outBomSize, const byte* buf, uint bufSize)
{
	// default to UTF8 Without Byte Order Mark
	outBomSize = 0;
	if (bufSize < 4)
		return TE_UTF8;

	// search for a byte-order mark.

	// UTF-32
	outBomSize = 4;
	const byte utf32[4] = { 0x00, 0x00, 0xFE, 0xFF };
	if (MemCmp(buf, utf32, 4) == 0)
		return TE_UTF32;

	// UTF-32 Little Endian
	outBomSize = 4;
	const byte utf32LE[4] = { 0xFE, 0xFF, 0x00, 0x00 };
	if (MemCmp(buf, utf32LE, 4) == 0)
		return TE_UTF32_LITTLE_ENDIAN;

	// UTF-16
	outBomSize = 2;
	const byte utf16[2] = { 0xFE, 0xFF };
	if (MemCmp(buf, utf16, 2) == 0)
		return TE_UTF16;

	// UTF-16 Little Endian
	outBomSize = 2;
	const byte utf16LE[2] = { 0xFF, 0xFE };
	if (MemCmp(buf, utf16LE, 2) == 0)
		return TE_UTF16_LITTLE_ENDIAN;

	// UTF-8
	outBomSize = 3;
	const byte utf8[3] = { 0xEF, 0xBB, 0xBF };
	if (MemCmp(buf, utf8, 3) == 0)
		return TE_UTF8_WITH_BOM;

	// default to UTF-8 Without Byte Order Mark
	outBomSize = 0;
	return TE_UTF8;
}


//===================
// StrUTF8CharLength
//===================
uint				StrUTF8CharLength(const byte* p, uint size)
{
    uint        len;
    int         i;
    int         m;

    if (*p == NUL)
        return 1;
    m = len = utf8len_tab[*p];
    if (len > size)
        m = size;       /* incomplete byte sequence. */
    for (i = 1; i < m; ++i)
        if ((p[i] & 0xc0) != 0x80)
            return 1;
    return len;
}


//===================
// StrUTF16toUTF8
//===================
uint				StrUTF16toUTF8(const wchar_t* utf16str, uint utf16len, byte* oututf8str)
{
    int         outlen = 0;
    int         todo = utf16len;
    const wchar_t* p = utf16str;
    int         l;
    int         ch, ch2;

    while (todo > 0)
    {
        ch = *p;
        if (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
        {
            /* surrogate pairs handling */
            ch2 = p[1];
            if (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
            {
                ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
                ++p;
                --todo;
            }
        }
        if (oututf8str != NULL)
        {
            l = utf_char2bytes(ch, oututf8str);
            oututf8str += l;
        }
        else
            l = utf_char2len(ch);
        ++p;
        outlen += l;
        --todo;
    }

    return outlen;
}


//===================
// StrUTF8toUTF16
//===================
uint				StrUTF8toUTF16(const char* utf8str, uint utf8len, wchar_t* oututf16str, int& unconvlen)
{
    int         outlen = 0;
    const char* p = utf8str;
    int         todo = utf8len;
    int         l;
    int         ch;

    while (todo > 0)
    {
        // Only convert if we have a complete sequence.
        l = utf_ptr2len_len(p, todo);
        if (l > todo)
        {
            // Return length of incomplete sequence.
			unconvlen = todo;
            break;
        }

        ch = utf_ptr2char(p);
        if (ch >= 0x10000)
        {
            // non-BMP character, encoding with surrogate pairs
            ++outlen;
            if (oututf16str != NULL)
            {
                *oututf16str++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
                *oututf16str++ = 0xDC00 | (ch & 0x3FF);
            }
        }
        else if (oututf16str != NULL)
            *oututf16str++ = ch;
        ++outlen;
        p += l;
        todo -= l;
    }

    return outlen;
}


//===================
// WStringToString
//===================
string			WStringToString(const wstring& str)
{
	if (str.empty())
		return SNULL;

	uint bufSize(StrUTF16toUTF8(str.c_str(), str.size(), NULL));
	if (bufSize == 0)
		return SNULL;

	byte* buf(TmpAlloc(bufSize+1));

	StrUTF16toUTF8(str.c_str(), str.size(), buf);
	buf[bufSize] = 0;
	string result((const char*)buf);

	TmpFree(buf);

	return result;
}


//===================
// StringToWString
//===================
wstring			StringToWString(const string& str)
{
	if (str.empty())
		return WSNULL;

	int unconv(0);
	uint bufSize(StrUTF8toUTF16(str.c_str(), str.size(), NULL, unconv));
	if (bufSize == 0)
		return WSNULL;

	byte* buf(TmpAlloc(bufSize+sizeof(wchar_t)));
	wchar_t* wbuf((wchar_t*)buf);

	StrUTF8toUTF16(str.c_str(), str.size(), wbuf, unconv);
	*((wchar_t*)(buf + bufSize)) = wchar_t(0);
	wstring result(wbuf);

	TmpFree(buf);

	return result;
}

