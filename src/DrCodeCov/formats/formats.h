#pragma once

#include "format_base.h"

void output_formats_init();
void output_formats_cleanup();

OutputFormatBase* output_format_find(const char *name);
