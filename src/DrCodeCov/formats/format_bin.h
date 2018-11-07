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
#ifdef WINDOWS
        header.checksum = mod.data->checksum;
        header.timestamp = mod.data->timestamp;
#else
        // FIXME: Use something else to identify the binary.
        header.checksum = 0x1c1c1c1c;
        header.timestamp = 0x2c2c2c2c;
#endif
        header.size = (uint32_t)(mod.getImageSize() * sizeof(Coverage_t));

        // Write header.
        dr_write_file(f, &header, sizeof(header));

        // Write bitmap.
        dr_write_file(f, mod.coverage, header.size);
    }

public:
    virtual const char* name() const override
    {
        return "bin";
    }

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
