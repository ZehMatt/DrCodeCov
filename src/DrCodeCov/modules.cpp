#include "modules.h"
#include "coverage.h"
#include "formats/formats.h"

#include <vector>
#include <algorithm>

// Variables.
static std::vector<ModuleEntry_t> _modules;
static void* _modLock = nullptr;

void modules_init()
{
    _modLock = dr_mutex_create();

    // Avoid all sorts of allocations, we just need it for managment.
    _modules.reserve(0x1000);
}

void modules_cleanup()
{
    dr_mutex_destroy(_modLock);
}

void modules_lock()
{
    dr_mutex_lock(_modLock);
}

void modules_unlock()
{
    dr_mutex_unlock(_modLock);
}

static ModuleEntry_t* module_find(app_pc va, bool requireLoaded)
{
    for(auto& mod : _modules)
    {
        if (requireLoaded && mod.loaded == false)
        {
            continue;
        }
        if (va >= mod.imageStart && va < mod.imageEnd)
        {
            return &mod;
        }
    }

    return nullptr;
}

static ModuleEntry_t* module_find_by_data(const module_data_t *data, bool requireLoaded)
{
    for (auto& mod : _modules)
    {
        if (requireLoaded && mod.loaded == false)
            continue;

        if (requireLoaded)
        {
            if (mod.data->start != data->start ||
                mod.data->end != data->end)
            {
                continue;
            }
        }

        if (mod.data->timestamp == data->timestamp &&
            mod.data->checksum == data->checksum &&
            mod.data->module_internal_size == data->module_internal_size &&
            mod.data->entry_point == data->entry_point)
        {
            return &mod;
        }
    }
    return nullptr;
}

void modules_tag_instr(void *drcontext, app_pc va, int len, bool isBranch)
{
    modules_lock();

    ModuleEntry_t* mod = module_find(va, true);

    if (!mod)
    {
        modules_unlock();
        return;
    }

    size_t offset = (va - mod->imageStart);
    mod->coverage[offset].flags |= Coverage_t::INSTR_START;

    if(isBranch)
        mod->coverage[offset].flags |= Coverage_t::BRANCH;

    for (int i = 1; i < len; i++)
    {
        mod->coverage[offset + i].flags |= Coverage_t::INSTR_PART;
    }

    modules_unlock();
}

void modules_add(void *drcontext, const module_data_t *info, bool loaded)
{
    modules_lock();

    //dr_messagebox("Module load %s\n", info->full_path);

    ModuleEntry_t *entry = module_find_by_data(info, false);
    if (!entry)
    {
        // New entry.
        size_t idx = _modules.size();
        _modules.resize(idx + 1);

        entry = &_modules[idx];
        entry->imageStart = info->start;
        entry->imageEnd = info->end;
        entry->data = dr_copy_module_data(info);

        const size_t coverageSize = sizeof(Coverage_t) * entry->getImageSize();
        entry->coverage = (Coverage_t*)dr_global_alloc(coverageSize);

        memset(entry->coverage, 0, coverageSize);
    }
    else
    {
        // Update only.
        entry->imageStart = info->start;
        entry->imageEnd = info->end;
    }

    entry->loaded = true;

    // FIXME: Sort everything for binary search.
    /*
    std::sort(_modules.begin(), _modules.end(), [](const ModuleEntry_t& a, const ModuleEntry_t& b)->bool
    {
        return a.imageStart < b.imageStart;
    });
    */

    modules_unlock();
}

void module_remove(void *drcontext, const module_data_t *info)
{
    modules_lock();

    ModuleEntry_t *mod = module_find_by_data(info, true);
    if (mod)
    {
        mod->loaded = false;
    }

    modules_unlock();
}

void modules_dump(const std::string& format, const std::string& outputDir)
{
    OutputFormatBase* fmtOut = output_format_find(format.c_str());

    if (!fmtOut)
    {
        dr_messagebox("Invalid output format selected: %s", format.c_str());
        return;
    }

    fmtOut->setOutputDirectory(outputDir);
    fmtOut->createOutput(_modules);
}

