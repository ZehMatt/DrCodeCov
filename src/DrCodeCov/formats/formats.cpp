#include "formats.h"
#include "format_bin.h"
#include "format_drcov.h"
#include "format_idc.h"

#include <memory>
#include <vector>
#include <algorithm>

static std::vector<std::unique_ptr<OutputFormatBase>> _outputFormats;

void output_formats_init()
{
    _outputFormats.emplace_back( std::make_unique<OutputFormatBinary>() );
    _outputFormats.emplace_back(std::make_unique<OutputFormatIDC>() );
    _outputFormats.emplace_back(std::make_unique<OutputFormatDrCov>() );
}

void output_formats_cleanup()
{
    _outputFormats.clear();
}

OutputFormatBase* output_format_find(const char *name)
{
    auto it = std::find_if(_outputFormats.begin(), _outputFormats.end(), [name](const std::unique_ptr<OutputFormatBase>& fmt)->bool
    {
        return _stricmp(fmt->name(), name) == 0;
    });

    if(it == _outputFormats.end())
        return nullptr;

    return (*it).get();
}
