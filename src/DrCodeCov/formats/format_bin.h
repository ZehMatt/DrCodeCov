#pragma once

#include "format_base.h"
#include "../coverage.h"
#include "../modules.h"

class OutputFormatBinary : public OutputFormatBase
{
private:
    void dumpModule(file_t f, const ModuleEntry_t& mod)
    {
        CoverageHeader_t header;
        header.magic = CoverageHeader_t::k_Magic;
        header.imageStart = (uint64_t)mod.imageStart;
        header.imageEnd = (uint64_t)mod.imageEnd;
        header.size = (uint32_t)(mod.getImageSize() * sizeof(Coverage_t));

        dr_write_file(f, &header, sizeof(header));
        dr_write_file(f, mod.coverage, header.size);
    }

public:
    virtual bool createOutput(const std::vector<ModuleEntry_t>& modules) override
    {
        char outFile[1024] = {};

        std::string outputFolder = getOutputDirectory();
        dr_create_dir(outputFolder.c_str());

        for (auto& mod : modules)
        {
            std::string modName = getModuleFileName(mod.data->full_path);
            dr_snprintf(outFile, 1024, "%s\\%s_bin.cov", outputFolder.c_str(), modName.c_str());

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
