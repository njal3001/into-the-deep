#include "debug.h"
#include <stdarg.h>
#include <stdio.h>

namespace ITD {

void Log::info(const char *format, ...)
{
	char msg[MESSAGE_MAX];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * MESSAGE_MAX, format, ap);
	va_end(ap);

	printf("INFO: %s\n", msg);
}

void Log::warn(const char *format, ...)
{
	char msg[MESSAGE_MAX];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * MESSAGE_MAX, format, ap);
	va_end(ap);

	printf("WARN: %s\n", msg);
}

void Log::error(const char *format, ...)
{
	char msg[MESSAGE_MAX];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(char) * MESSAGE_MAX, format, ap);
	va_end(ap);

	printf("ERROR: %s\n", msg);
}

}  // namespace ITD
