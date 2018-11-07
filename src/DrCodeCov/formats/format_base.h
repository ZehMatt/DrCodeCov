#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#include "dr_api.h"
#include "drmgr.h"

struct ModuleEntry_t;

class OutputFormatBase
{
private:
    std::string _outputDirectory;

public:
    void setOutputDirectory(const std::string& outputDir)
    {
        _outputDirectory = outputDir;
    }

    virtual const char* name() const = 0;

protected:
    const std::string& getOutputDirectory() const
    {
        return _outputDirectory;
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

