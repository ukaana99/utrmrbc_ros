/**
*****************************************************************************
**
**  File        : tiny_printf.c
**
**  Abstract    : Atollic TrueSTUDIO Minimal iprintf/siprintf/fiprintf
**                and puts/fputs.
**                Provides aliased declarations for printf/sprintf/fprintf
**                pointing to *iprintf variants.
**
**                The argument contains a format string that may include
**                conversion specifications. Each conversion specification
**                is introduced by the character %, and ends with a
**                conversion specifier.
**
**                The following conversion specifiers are supported
**                cdisuxX%
**
**                Usage:
**                c    character
**                d,i  signed integer (-sign added, + sign not supported)
**                s    character string
**                u    unsigned integer as decimal
**                x,X  unsigned integer as hexadecimal (uppercase letter)
**                %    % is written (conversion specification is '%%')
**
**                Note:
**                Character padding is not supported
**
**  Environment : Atollic TrueSTUDIO
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/
/* Includes */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Create aliases for *printf to integer variants *iprintf */
__attribute__ ((alias("iprintf"))) int printf(const char *fmt, ...);
__attribute__ ((alias("fiprintf"))) int fprintf(FILE* fp, const char *fmt, ...);
__attribute__ ((alias("siprintf"))) int sprintf(char* str, const char *fmt, ...);

/* External function prototypes (defined in syscalls.c) */
extern int _write(int fd, char *str, int len);

/* Private function prototypes */
void ts_itoa(char **buf, int d, int base);
int ts_formatstring(char *buf, const char *fmt, va_list va);
int ts_formatlength(const char *fmt, va_list va);

void reverse(char *str, int len);
/* Private functions */

/**
**---------------------------------------------------------------------------
**  Abstract: Convert integer to ascii
**  Returns:  void
**---------------------------------------------------------------------------
*/
void ts_itoa(char **buf, int d, int base)
{
	int div = 1;
	while (d/div >= base)
		div *= base;

	while (div != 0)
	{
		int num = d/div;
		d = d%div;
		div /= base;
		if (num > 9)
			*((*buf)++) = (num-10) + 'A';
		else
			*((*buf)++) = num + '0';
	}
}
/**
**---------------------------------------------------------------------------
**  Abstract: Writes arguments va to buffer buf according to format fmt
**  Returns:  Length of string
**---------------------------------------------------------------------------
*/
int ts_formatstring(char *buf, const char *fmt, va_list va)
{
	char *start_buf = buf;
	int precision = 6; /* default precision for float printing is 6 */

	while(*fmt)
	{
		/* Character needs formating? */
		if (*fmt == '%')
		{
			switch (*(++fmt))
			{
			  case 'c':
				*buf++ = va_arg(va, int);
				break;
			  case 'd':
			  case 'i':
				{
					signed int val = va_arg(va, signed int);
					if (val < 0)
					{
						val *= -1;
						*buf++ = '-';
					}
					ts_itoa(&buf, val, 10);
				}
				break;
			  case 's':
				{
					char * arg = va_arg(va, char *);
					while (*arg)
					{
						*buf++ = *arg++;
					}
				}
				break;
			  case 'u':
					ts_itoa(&buf, va_arg(va, unsigned int), 10);
				break;
			  case 'l':
				    ++fmt;  /* to cancel 'd' printing */
					long val = va_arg(va, long);
					if (val < 0)
					{
						val *= -1;
						*buf++ = '-';
					}
					ts_itoa(&buf, val, 10);
				break;
			  case 'x':
			  case 'X':
					ts_itoa(&buf, va_arg(va, int), 16);
				break;
			  case '.':
				  precision = *(++fmt) - '0';
				  if(!(precision >= 0 && precision <= 6))
					  precision = 6;
				  ++fmt;
			  case 'f':
				{

		/**********edited by Kai Sheng and Qiu Hui on 27/7/2017**************/
					float fval;
					fval = va_arg(va, double);
					if(fval < 0.0) {
						fval *= -1.0;
						*buf++ = '-';
					}
					long heiltal = (long)fval;

					int i = 0;

					if(heiltal == 0){
						*buf++ = '0';
						i++;
					}
					while (heiltal)
					{
						*buf++ = (heiltal%10) + '0';
						heiltal = heiltal/10;
						i++;
					}

					reverse(buf - i, i);

					heiltal = (long)fval;
					*buf++ = '.';

					float frac = fval - (float)heiltal;
					long decimal = (long)(frac * pow(10,precision));

					int j = 0;

					while (decimal)
					{
						*buf++ = (decimal % 10) + '0';
						decimal = decimal / 10;
						j++;
					}

					 while (j < precision){
						 *buf++ = '0';
						 j++;
					 }

					reverse(buf - j, j);
					precision = 6;
				 }
		/*************************************************************************/
				 break;
			  case '%':
				  *buf++ = '%';
				  break;
			}
			fmt++;
		}
		/* Else just copy */
		else
		{
			*buf++ = *fmt++;
		}
	}
	*buf = 0;

	return (int)(buf - start_buf);
}


/**
**---------------------------------------------------------------------------
**  Abstract: Calculate maximum length of the resulting string from the
**            format string and va_list va
**  Returns:  Maximum length
**---------------------------------------------------------------------------
*/
int ts_formatlength(const char *fmt, va_list va)
{
	int length = 0;
	while (*fmt)
	{
		if (*fmt == '%')
		{
			++fmt;
			switch (*fmt)
			{
			  case 'c':
		  		  va_arg(va, int);
				  ++length;
				  break;
			  case 'd':
			  case 'i':
			  case 'u':
				  /* 32 bits integer is max 11 characters with minus sign */
				  length += 11;
				  va_arg(va, int);
				  break;
			  case 's':
			  	  {
			  		  char * str = va_arg(va, char *);
			  		  while (*str++)
			  			  ++length;
			  	  }
				  break;
			  case 'x':
			  case 'X':
				  /* 32 bits integer as hex is max 8 characters */
				  length += 8;
				  va_arg(va, unsigned int);
				  break;
			  default:
				  ++length;
				  break;
			}
		}
		else
		{
			++length;
		}
		++fmt;
	}
	return length;
}

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            given character string according to the format parameter.
**  Returns:  Number of bytes written
**===========================================================================
*/
int siprintf(char *buf, const char *fmt, ...)
{
	int length;
	va_list va;
	va_start(va, fmt);
	length = ts_formatstring(buf, fmt, va);
	va_end(va);
	return length;
}

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            given file stream according to the format parameter.
**  Returns:  Number of bytes written
**===========================================================================
*/
int fiprintf(FILE * stream, const char *fmt, ...)
{
	int length = 0;
	va_list va;
	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	va_end(va);
	{
		char buf[length];
		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);
		length = _write(stream->_file, buf, length);
		va_end(va);
	}
	return length;
}

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            standard output according to the format parameter.
**  Returns:  Number of bytes written
**
**===========================================================================
*/
int iprintf(const char *fmt, ...)
{
	int length = 0;
	va_list va;
	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	va_end(va);
	{
		char buf[length];
		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);
		length = _write(1, buf, length);
		va_end(va);
	}
	return length;
}

/**
**===========================================================================
**  Abstract: fputs writes the string at s (but without the trailing null) to
**  the file or stream identified by fp.
**  Returns:  If successful, the result is 0; otherwise, the result is EOF.
**
**===========================================================================
*/
int fputs(const char *s, FILE *fp)
{
	int length = strlen(s);
	int wlen = 0;
	int res;

	wlen = _write((fp->_file), (char*)s, length);
	wlen += _write((fp->_file), "\n", 1);

	if (wlen == (length+1))
	{
		res = 0;
	}
	else
	{
		res = EOF;
	}

	return res;
}

/**
**===========================================================================
**  Abstract: puts writes the string at s (followed by a newline, instead of
**  the trailing null) to the standard output stream.
**  Returns:  If successful, the result is a nonnegative integer; otherwise,
**  the result is EOF.
**
**===========================================================================
*/
int puts(const char *s)
{
	int length = strlen(s);
	int numbytes = 0;
	int res;

	numbytes = _write(1, (char*)s, length);
	numbytes += _write(1, "\n", 1);

	if (numbytes == (length+1))
	{
		res = 0;
	}
	else
	{
		res = EOF;
	}

	return res;
}

/**
**===========================================================================
**  Abstract: Copy, starting from the memory location buf, count elements
**  (each of size size) into the file or stream identified by fp.
**  Returns:  Number of elements written
**
**===========================================================================
*/
size_t fwrite(const void * buf, size_t size, size_t count, FILE * fp)
{
	return (_write((fp->_file), (char*)buf, size * count) / size);
}

/**********edited by Kai Sheng and Qiu Hui on 27/7/2017**************/
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
/*********************************************************************/
