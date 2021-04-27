#include "il2cpp-config.h"

#include <scebase.h>
#include <kernel.h>
#include <app_content.h>
#include <libsysmodule.h>

#include "os/Initialize.h"
#include "utils/Logging.h"

static SceAppContentInitParam s_initParam;
static SceAppContentBootParam s_bootParam;
bool g_TemporaryDataIsMounted = false;
SceAppContentMountPoint g_TemporaryDataMountPoint;

void il2cpp::os::Initialize()
{
    if (sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT) != SCE_OK)
    {
        utils::Logging::Write("InitializeAppContent failed");
        return;
    }

    /* Set parameters to be passed to the initialization function sceAppContentInitialize()*/
    memset(&s_initParam, 0, sizeof(SceAppContentInitParam));
    memset(&s_bootParam, 0, sizeof(SceAppContentBootParam));

    /* Perform library initialization processing */
    int result = sceAppContentInitialize(&s_initParam, &s_bootParam);
    if (result != SCE_OK)
    {
        utils::Logging::Write("sceAppContentInitialize returned 0x%x\n", result);
        return;
    }

    /* Mount temporary data */
    int ret = sceAppContentTemporaryDataMount2(SCE_APP_CONTENT_TEMPORARY_DATA_OPTION_NONE, &g_TemporaryDataMountPoint);
    if (ret == 0)
        g_TemporaryDataIsMounted = true;
}

void il2cpp::os::Uninitialize()
{
}
