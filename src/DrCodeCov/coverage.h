#pragma once

#include <stdint.h>

#pragma pack(push, 1)
struct CoverageHeader_t
{
    enum { k_Magic = 0xDEADBEEF };

    uint32_t magic;
    uint32_t size;
    uint64_t imageStart;
    uint64_t imageEnd;
    uint32_t timestamp;
    uint32_t checksum;
};

struct Coverage_t
{
    enum
    {
        UNREACHED = 0,                      // Unreached.
        INSTR_START = (1 << 0),             // Begin of instruction
        INSTR_PART = (1 << 1),              // Part of instruction.
        BRANCH_START = (1 << 2),            // Branch starts with this instructions.
        BRANCH_END = (1 << 3),              // Branch ends with this instructions.
    };
    uint8_t flags;
};

#pragma pack(pop)
