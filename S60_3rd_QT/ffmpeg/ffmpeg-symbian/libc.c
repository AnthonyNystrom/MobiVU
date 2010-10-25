#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

void snprintf(char* str, size_t size, const char* format, ...) {
	va_list ap;

	va_start(ap, format);
	vsprintf(str, format, ap);
	va_end(ap);
}

int mkstemp(char *template) {
	return -1;
}

long long llrint(double x) {
	if (x > 0)
		return (long long) (x + 0.5);
	else
		return (long long) (x - 0.5);
}


