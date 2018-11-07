#pragma once

#include "format_base.h"
#include "../coverage.h"
#include "../modules.h"

class OutputFormatIDC : public OutputFormatBase
{
private:
    void dumpModule(file_t f, const ModuleEntry_t& mod)
    {
        dr_fprintf(f, "#include <ida.idc>\n"
            "static main() {\n");
#ifdef _WIN64
        dr_fprintf(f, "\tauto imageBase = 0x%.16llX;\n", (void*)mod.imageStart);
#else
        dr_fprintf(f, "\tauto imageBase = 0x%08X;\n", (void*)mod.imageStart);
#endif
        size_t reps = 0;
        for (size_t n = 0; n < mod.getImageSize(); n++)
        {
            if (mod.coverage[n].flags & Coverage_t::BRANCH)
            {
                uint32_t rva = (uint32_t)(n);
                dr_fprintf(f, "\tMakeCode(imageBase+0x%08X);\n", rva);
            }
        }
        dr_fprintf(f, "}\n");
    }

public:
    virtual const char* name() const override
    {
        return "idc";
    }

    virtual bool createOutput(const std::vector<ModuleEntry_t>& modules) override
    {
        char outFile[1024] = {};

        std::string outputFolder = getOutputDirectory();
        dr_create_dir(outputFolder.c_str());

        for (auto& mod : modules)
        {
            std::string modName = getModuleFileName(mod.data->full_path);
            dr_snprintf(outFile, 1024, "%s\\%s.idc", outputFolder.c_str(), modName.c_str());

            file_t f = dr_open_file(outFile, DR_FILE_WRITE_REQUIRE_NEW);
            if (f == INVALID_FILE)
            {
                dr_fprintf(STDERR, "Unable to open file for writing: %s", outFile);
                continue;
            }

            dumpModule(f, mod);

            dr_close_file(f);
        }

        return true;
    }
};
