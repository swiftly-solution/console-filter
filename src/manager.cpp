#include "entrypoint.h"

void SwiftlyConFilterManagerHelp()
{
    g_SMAPI->ConPrint("[Console Filter] Swiftly Console Filter Menu\n");
    g_SMAPI->ConPrint("[Console Filter] Usage: sw_confilter <command>\n");
    g_SMAPI->ConPrint("[Console Filter]  disable    - Disables the Console Filter.\n");
    g_SMAPI->ConPrint("[Console Filter]  enable     - Enables the Console Filter.\n");
    g_SMAPI->ConPrint("[Console Filter]  reload     - Reloads the Console Filter messages.\n");
    g_SMAPI->ConPrint("[Console Filter]  stats      - Shows the console filter stats.\n");
}

void SwiftlyConFilterEnable()
{
    if (g_Ext.Status())
        return g_SMAPI->ConPrint("[Console Filter] Console Filter is already enabled.\n");

    g_Ext.Toggle();
    g_SMAPI->ConPrint("[Console Filter] Console Filter has been enabled.\n");
}

void SwiftlyConFilterDisable()
{
    if (!g_Ext.Status())
        return g_SMAPI->ConPrint("[Console Filter] Console Filter is already disabled.\n");

    g_Ext.Toggle();
    g_SMAPI->ConPrint("[Console Filter] Console Filter has been disabled.\n");
}

void SwiftlyConFilterStats()
{
    g_SMAPI->ConPrintf("[Console Filter] Console Filter status: %s.\n", g_Ext.Status() ? "Enabled" : "Disabled");
    g_SMAPI->ConPrint("[Console Filter] Below it will be shown the amount of messages filtered:\n");
    std::map<std::string, uint64> counters = g_Ext.GetCounters();
    uint32 idx = 0;
    for (std::map<std::string, uint64>::iterator it = counters.begin(); it != counters.end(); ++it)
    {
        ++idx;
        g_SMAPI->ConPrintf("[Console Filter] %02d. %s -> %llu\n", idx, it->first.c_str(), it->second);
    }
}

void SwiftlyConFilterReload()
{
    bool shouldRestart = g_Ext.Status();

    if (g_Ext.Status())
        g_Ext.Toggle();

    g_Ext.LoadFilters();
    g_SMAPI->ConPrint("[Console Filter] Console Filter messages have been succesfully reloaded.\n");

    if (shouldRestart)
        g_Ext.Toggle();
}

void SwiftlyConFilterManager(const char* subcmd)
{
    std::string sbcmd = subcmd;
    if (sbcmd.size() == 0)
    {
        SwiftlyConFilterManagerHelp();
        return;
    }

    if (sbcmd == "enable")
        SwiftlyConFilterEnable();
    else if (sbcmd == "disable")
        SwiftlyConFilterDisable();
    else if (sbcmd == "stats")
        SwiftlyConFilterStats();
    else if (sbcmd == "reload")
        SwiftlyConFilterReload();
    else
        SwiftlyConFilterManagerHelp();
}

CON_COMMAND(sw_confilter, "Console Filter Menu")
{
    SwiftlyConFilterManager(args[1]);
}