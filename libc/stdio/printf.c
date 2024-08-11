#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// not thread safe!
static int chars_printed = 0;

static bool print(const char *data, size_t length)
{
	const unsigned char *bytes = (const unsigned char *)data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	chars_printed++;
	return true;
}

static inline bool print_integer(int n)
{
	char digs[9];
	int num_digs = 0;

	if (n < 0) {
		if (!print("-", 1))
			return false;
		n *= -1;
	}

	if (n == 0)
		return print("0", 1);

	while (n > 0) {
		char dig = (n % 10) + '0';
		digs[8 - num_digs] = dig;
		n /= 10;
		num_digs++;
	}

	const char *start = digs + (9 - num_digs);
	return print(start, num_digs);
}

static inline bool print_hex_unsigned(unsigned n)
{
	char digs[8];
	int num_digs = 0;

	if (!print("0x", 2))
		return false;

	if (n == 0)
		return print("0", 1);

	while (n > 0) {
		unsigned modulus = n % 16;
		char dig;
		if (modulus < 10) {
			dig = '0' + modulus;
		} else {
			dig = modulus - 10 + 'A';
		}

		digs[7 - num_digs] = dig;
		n /= 16;
		num_digs++;
	}

	const char *start = digs + (8 - num_digs);
	return print(start, num_digs);
}

enum States {
	NORMAL = 0,
	FORMAT_SPECIFIER,
};

int printf(const char *restrict format, ...)
{
	va_list parameters;
	int fmt_int;
	unsigned fmt_unsigned;
	const char *fmt_str;
	char fmt_char;
	va_start(parameters, format);

	enum States curr_state = NORMAL;

	while (*format != '\0') {
		if (*format == '%') {
			switch (curr_state) {
			case NORMAL:
				curr_state = FORMAT_SPECIFIER;
				break;
			case FORMAT_SPECIFIER:
				if (!print("%", 1))
					return -1;
				break;
			}
		} else {
			if (curr_state == FORMAT_SPECIFIER) {
				switch (*format) {
				case 'd':
					// print integer
					fmt_int = va_arg(parameters, int);
					if (!print_integer(fmt_int))
						return -1;
					break;
				case 'c':
					// print character
					fmt_char = va_arg(parameters, int);
					if (!print(&fmt_char, 1))
						return -1;
					break;
				case 's':
					// print string
					fmt_str = va_arg(parameters,
							 const char *);
					if (!print(fmt_str, strlen(fmt_str)))
						return -1;
					break;
				case 'x':
					fmt_unsigned =
						va_arg(parameters, unsigned);
					if (!print_hex_unsigned(fmt_unsigned))
						return -1;
					break;
				default:
					// undefined format specifier
					return -1;
				}
				curr_state = NORMAL;
			} else {
				if (!print(format, 1))
					return -1;
			}
		}
		format++;
	}

	int tmp = chars_printed;
	chars_printed = 0;
	return tmp;
}
