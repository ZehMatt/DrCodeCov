#pragma once

#include "format_base.h"
#include "../coverage.h"
#include "../modules.h"

#include <list>

#pragma pack(push, 1)
struct drcov_bb
{
    uint32_t start;
    uint16_t size;
    uint16_t id;
};
#pragma pack(pop)

class OutputFormatDrCov : public OutputFormatBase
{
private:
    void writeHeader(file_t f, size_t numOfModules)
    {
        dr_fprintf(f, "DRCOV VERSION: 2\n");
        dr_fprintf(f, "DRCOV FLAVOR: drcov\n");
        dr_fprintf(f, "Module Table: version 2, count %u\n", (uint32_t)numOfModules);
        dr_fprintf(f, "Columns: id, base, end, entry, checksum, timestamp, path\n");
    }

    void writeModuleData(file_t f, const std::vector<ModuleEntry_t>& modules)
    {
        uint32_t cnt = 0;
        for (auto &i : modules)
        {
            std::string moduleName = i.data->full_path;
            void * startAddr = i.imageStart;
            void * endAddr = i.imageEnd;
            void * entryPoint = i.data->entry_point;
            uint32_t checksum = i.data->checksum;
            uint32_t timestamp = i.data->timestamp;

            dr_fprintf(f, "%2u, %p, %p, %p, 0x%x, 0x%x, %s\n",
                cnt, startAddr, endAddr, entryPoint, checksum, timestamp, moduleName.c_str());

            cnt++;
        }
    }

    uint16_t countBlockSize(Coverage_t *coverageArray, size_t index, size_t arraySize)
    {
        uint16_t size = 1;

        size_t maxIterations = arraySize - index;
        for (size_t i = 1; i < maxIterations; i++, size++)
        {
            if (coverageArray[index + i].flags & Coverage_t::BRANCH ||
                coverageArray[index + i].flags == Coverage_t::UNREACHED)
            {
                break;
            }
        }

        return size;
    }

    void writeBasicBlockData(file_t f, const std::vector<ModuleEntry_t>& modules)
    {
        std::list<drcov_bb> drBasicBlocks;

        uint32_t modId = 0;
        for (auto &mod : modules)
        {
            drcov_bb bbl;

            for (size_t n = 0; n < mod.getImageSize(); n++)
            {
                bbl.id = 0;
                bbl.start = 0;
                bbl.size = 0;

                if (mod.coverage[n].flags & Coverage_t::BRANCH)
                {
                    bbl.id = modId;                // Module Id
                    bbl.start = (uint32_t)(n);     // RVA
                    bbl.size = countBlockSize(mod.coverage, n, mod.getImageSize());
                    drBasicBlocks.push_back(bbl);

                    n = n + bbl.size - 1;
                }
            }

            modId++;
        }

        dr_fprintf(f, "BB Table: %u bbs\n", drBasicBlocks.size());

        for (auto &bbl : drBasicBlocks)
        {
            dr_write_file(f, &bbl, sizeof(bbl));
        }
    }

    void writeDrCovFile(file_t f, const std::vector<ModuleEntry_t>& modules)
    {
        writeHeader(f, modules.size());
        writeModuleData(f, modules);
        writeBasicBlockData(f, modules);
    }

public:
    static OutputFormatBase* instance()
    {
        static OutputFormatDrCov fmt;
        return &fmt;
    }

    virtual bool createOutput(const std::vector<ModuleEntry_t>& modules) override
    {
        char outFile[1024] = {};
        uint32_t pid = (uint32_t)dr_get_process_id();

        std::string outputFolder = getOutputDirectory();
        dr_create_dir(outputFolder.c_str());

        dr_snprintf(outFile, 1024, "%s\\proc_%d.cov", outputFolder.c_str(), pid);
        file_t f = dr_open_file(outFile, DR_FILE_WRITE_REQUIRE_NEW);
        if (f == INVALID_FILE)
        {
            dr_fprintf(STDERR, "Unable to open file for writing: %s", outFile);
            return false;
        }

        writeDrCovFile(f, modules);

        dr_close_file(f);

        return true;
    }
};
