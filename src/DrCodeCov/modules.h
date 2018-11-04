#pragma once

#include <stdint.h>

#include "dr_api.h"
#include "drmgr.h"
#include "coverage.h"

struct ModuleEntry_t
{
    app_pc imageStart;
    app_pc imageEnd;

    Coverage_t *coverage;
	module_data_t *data;
	bool loaded;

    size_t getImageSize() const
    {
        return (size_t)(imageEnd - imageStart);
    }
};

void modules_init();
void modules_cleanup();
void modules_lock();
void modules_unlock();

void modules_add(void *drcontext, const module_data_t *info, bool loaded);
void module_remove(void *drcontext, const module_data_t *info);

void modules_tag_instr(void *drcontext, app_pc va, int len, bool isBranch);
void modules_dump();
