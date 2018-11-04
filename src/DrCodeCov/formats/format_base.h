#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#include "dr_api.h"
#include "drmgr.h"

struct ModuleEntry_t;

class OutputFormatBase
{
protected:
    std::string getOutputDirectory() const
    {
        uint32_t pid = (uint32_t)dr_get_process_id();

        char curDir[1024] = {};
        dr_get_current_directory(curDir, 1024);

        char outPath[1024] = {};
        dr_snprintf(outPath, 1024, "%s\\coverage.%08X", curDir, pid);

        return outPath;
    }

    std::string getModuleFileName(const char *fullPath) const
    {
        char moduleName[1024];
        dr_snprintf(moduleName, 1024, "%s", fullPath);

        // Sanitize slashes
        for (auto& c : moduleName)
        {
            if (c == '/')
                c = '\\';
        }

        const char *modName = strrchr(moduleName, '\\');
        if (modName)
        {
            dr_snprintf(moduleName, 1024, "%s", modName + 1);
        }
        else
        {
            dr_snprintf(moduleName, 1024, "%s", fullPath);
        }

        return moduleName;
    }

public:
    virtual bool createOutput(const std::vector<ModuleEntry_t>& modules) = 0;
};

