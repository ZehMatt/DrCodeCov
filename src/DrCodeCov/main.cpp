#include "dr_api.h"
#include "drmgr.h"
#include "modules.h"
#include "droption.h"

static droption_t<std::string> op_format(DROPTION_SCOPE_CLIENT, 
    "format", 
    "", 
    "Output Format",
    "Possible options: bin <default>, idc, drcov");

static void event_module_load(void *drcontext, const module_data_t *info, bool loaded)
{
    modules_add(drcontext, info, loaded);
}

static void event_module_unload(void *drcontext, const module_data_t *info)
{
    module_remove(drcontext, info);
}

static void event_exit(void)
{
    std::string fmt = op_format.get_value();
    if (fmt.empty())
        fmt = "bin";

    modules_dump(fmt);
    modules_cleanup();
    drmgr_exit();
}

static dr_emit_flags_t event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr, bool for_trace, bool translating, void *user_data)
{
    if (!instr_is_app(instr))
        return DR_EMIT_DEFAULT;

    bool isBranch = drmgr_is_first_instr(drcontext, instr);

    int instrLength = instr_length(drcontext, instr);
    app_pc va = instr_get_app_pc(instr);

    modules_tag_instr(drcontext, va, instrLength, isBranch);

    return DR_EMIT_DEFAULT;
}

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[])
{
    dr_set_client_name("DrCodeCoverage", "");

    std::string parse_err;

    if (!droption_parser_t::parse_argv(DROPTION_SCOPE_CLIENT, argc, argv, &parse_err, nullptr))
    {
        dr_fprintf(STDERR, "Usage error: %s", parse_err.c_str());
        dr_abort();
    }

    std::string fmt = op_format.get_value();
    if(fmt.empty())
        fmt = "bin";

    if (fmt != "bin" && fmt != "drcov" && fmt != "idc")
    {
        dr_fprintf(STDERR, "Invalid output format: %s", fmt.c_str());
        dr_abort();
    }

    drmgr_init();
    dr_register_exit_event(event_exit);

    if (!drmgr_register_bb_instrumentation_event(NULL, event_app_instruction, NULL))
    {
        dr_messagebox("drmgr_register_bb_instrumentation_event failed");
    }
    if (!drmgr_register_module_load_event(event_module_load))
    {
        dr_messagebox("drmgr_register_module_load_event failed");
    }
    if (!drmgr_register_module_unload_event(event_module_unload))
    {
        dr_messagebox("drmgr_register_module_unload_event failed");
    }

    modules_init();
}

