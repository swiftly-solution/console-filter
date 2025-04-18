#ifndef _entrypoint_h
#define _entrypoint_h

#include <string>

#include <swiftly-ext/core.h>
#include <swiftly-ext/extension.h>
#include <swiftly-ext/hooks/function.h>
#include <swiftly-ext/hooks/vfunction.h>

#include <regex>
#include <string>
#include <map>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>

class ConsoleFilter : public SwiftlyExt
{
private:
    bool m_status = false;
    std::map<std::string, std::regex> filter;
    std::map<std::string, uint64> counter;

public:
    bool Load(std::string& error, SourceHook::ISourceHook* SHPtr, ISmmAPI* ismm, bool late);
    bool Unload(std::string& error);

    void AllExtensionsLoaded();
    void AllPluginsLoaded();

    bool OnPluginLoad(std::string pluginName, void* pluginState, PluginKind_t kind, std::string& error);
    bool OnPluginUnload(std::string pluginName, void* pluginState, PluginKind_t kind, std::string& error);

    void LoadFilters();
    void Toggle();
    bool NeedFiltering(std::string message);
    std::map<std::string, uint64> GetCounters() { return this->counter; }

    bool Status() { return this->m_status; }

public:
    const char* GetAuthor();
    const char* GetName();
    const char* GetVersion();
    const char* GetWebsite();
};

template <typename... Args>
std::string string_format(const std::string& format, Args... args)
{
    int size_s = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    if (size_s <= 0)
        return "";

    size_t size = static_cast<size_t>(size_s);
    char* buf = new char[size];
    snprintf(buf, size, format.c_str(), args...);
    std::string out = std::string(buf, buf + size - 1);
    delete buf;
    return out;
}

extern ConsoleFilter g_Ext;
DECLARE_GLOBALVARS();

#endif