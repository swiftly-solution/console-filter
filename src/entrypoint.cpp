#include "entrypoint.h"
#include <swiftly-ext/config.h>
#include <fstream>
#include <tier1/convar.h>
#include <swiftly-ext/files.h>

//////////////////////////////////////////////////////////////
/////////////////        Core Variables        //////////////
////////////////////////////////////////////////////////////

ConsoleFilter g_Ext;
CREATE_GLOBALVARS();

//////////////////////////////////////////////////////////////
/////////////////             Hooks            //////////////
////////////////////////////////////////////////////////////

dyno::ReturnAction Hook_CLoggingSystem_LogDirect(dyno::CallbackType cbType, dyno::IHook& hook)
{
    if (!g_Ext.Status()) return dyno::ReturnAction::Ignored;

    va_list* args = hook.getArgument<va_list*>(7);
    const char* str = hook.getArgument<const char*>(6);
    char buf[MAX_LOGGING_MESSAGE_LENGTH];
    if (args) {
        va_list cpargs;
        va_copy(cpargs, *args);
        V_vsnprintf(buf, sizeof(buf), str, cpargs);
        va_end(cpargs);
    }

    if (g_Ext.NeedFiltering((args ? buf : str))) {
        hook.setReturn<int>(0);
        return dyno::ReturnAction::Supercede;
    }

    return dyno::ReturnAction::Ignored;
}

FunctionHook CLoggingSystem_LogDirect("CLoggingSystem_LogDirect", dyno::CallbackType::Pre, Hook_CLoggingSystem_LogDirect, "piipppsp", 'i');

//////////////////////////////////////////////////////////////
/////////////////          Core Class          //////////////
////////////////////////////////////////////////////////////

void ConFilterError(std::string text)
{
    if (!g_SMAPI)
        return;

    g_SMAPI->ConPrintf("[Console Filter] %s\n", text.c_str());
}

EXT_EXPOSE(g_Ext);
bool ConsoleFilter::Load(std::string& error, SourceHook::ISourceHook* SHPtr, ISmmAPI* ismm, bool late)
{
    SAVE_GLOBALVARS();

    GET_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);

    LoadFilters();

    if (FetchConfigValue<bool>("core.console_filtering"))
        Toggle();

    return true;
}

bool ConsoleFilter::Unload(std::string& error)
{
    return true;
}

void ConsoleFilter::AllExtensionsLoaded()
{

}

void ConsoleFilter::AllPluginsLoaded()
{

}

bool ConsoleFilter::OnPluginLoad(std::string pluginName, void* pluginState, PluginKind_t kind, std::string& error)
{
    return true;
}

bool ConsoleFilter::OnPluginUnload(std::string pluginName, void* pluginState, PluginKind_t kind, std::string& error)
{
    return true;
}

void ConsoleFilter::Toggle()
{
    m_status = !m_status;
}

void ConsoleFilter::LoadFilters()
{
    filter.clear();
    counter.clear();

    std::ifstream ifs(GeneratePath("addons/swiftly/configs/console_filter.json"));
    if (!ifs.is_open()) {
        ConFilterError("Failed to open 'addons/swiftly/configs/console_filter.json'.");
        return;
    }

    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document confilterFile;
    confilterFile.ParseStream(isw);

    if (confilterFile.HasParseError())
        return ConFilterError(string_format("A parsing error has been detected.\nError (offset %u): %s\n", (unsigned)confilterFile.GetErrorOffset(), GetParseError_En(confilterFile.GetParseError())));

    if (confilterFile.IsArray())
        return ConFilterError("Console filters file cannot be an array.");

    for (auto it = confilterFile.MemberBegin(); it != confilterFile.MemberEnd(); ++it)
    {
        std::string key = it->name.GetString();

        if (!it->value.IsString())
        {
            ConFilterError(string_format("The field \"%s\" is not a string.", key));
            continue;
        }

        try
        {
            std::regex tmp(it->value.GetString(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs);
        }
        catch (const std::regex_error& err)
        {
            ConFilterError(string_format("The regex for \"%s\" is not valid.", key.c_str()));
            ConFilterError(string_format("Error: %s", err.what()));
            continue;
        }

        filter.insert({ key, std::regex(it->value.GetString(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs) });
        counter.insert({ key, 0 });
    }
}

bool ConsoleFilter::NeedFiltering(std::string message)
{
    if (!Status())
        return false;

    for (auto it = filter.begin(); it != filter.end(); ++it)
    {
        std::regex& val = it->second;

        if (std::regex_search(message, val))
        {
            std::string& key = it->first;
            counter[key]++;
            return true;
        }
    }

    return false;
}

const char* ConsoleFilter::GetAuthor()
{
    return "Swiftly Development Team";
}

const char* ConsoleFilter::GetName()
{
    return "Console Filter";
}

const char* ConsoleFilter::GetVersion()
{
#ifndef VERSION
    return "Local";
#else
    return VERSION;
#endif
}

const char* ConsoleFilter::GetWebsite()
{
    return "https://swiftlycs2.net/";
}
