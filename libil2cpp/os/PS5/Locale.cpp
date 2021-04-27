#include "il2cpp-config.h"

#include <system_service.h>
#include "os/Locale.h"

namespace il2cpp
{
namespace os
{
    std::string Locale::GetLocale()
    {
        int32_t language = 1;
        const int numLangs = 23;
        const char * LocaleNames[numLangs] =
        {
            "ja", //    SCE_SYSTEM_PARAM_LANG_JAPANESE
            "en-us",//  SCE_SYSTEM_PARAM_LANG_ENGLISH_US
            "fr", //    SCE_SYSTEM_PARAM_LANG_FRENCH
            "es", //    SCE_SYSTEM_PARAM_LANG_SPANISH
            "de", //    SCE_SYSTEM_PARAM_LANG_GERMAN
            "it", //    SCE_SYSTEM_PARAM_LANG_ITALIAN
            "nl", //    SCE_SYSTEM_PARAM_LANG_DUTCH
            "pt", //  SCE_SYSTEM_PARAM_LANG_PORTUGUESE_PT
            "ru", //    SCE_SYSTEM_PARAM_LANG_RUSSIAN
            "ko", //    SCE_SYSTEM_PARAM_LANG_KOREAN
            "zh", //    SCE_SYSTEM_PARAM_LANG_CHINESE_T
            "zh", //    SCE_SYSTEM_PARAM_LANG_CHINESE_S
            "fi", //    SCE_SYSTEM_PARAM_LANG_FINNISH
            "sv", //    SCE_SYSTEM_PARAM_LANG_SWEDISH
            "da", //    SCE_SYSTEM_PARAM_LANG_DANISH
            "no", //    SCE_SYSTEM_PARAM_LANG_NORWEGIAN
            "pl", //    SCE_SYSTEM_PARAM_LANG_POLISH
            "pt-br", // SCE_SYSTEM_PARAM_LANG_PORTUGUESE_BR
            "en-gb", // SCE_SYSTEM_PARAM_LANG_ENGLISH_GB
            "tr", //    SCE_SYSTEM_PARAM_LANG_TURKISH
            "es", //    SCE_SYSTEM_PARAM_LANG_SPANISH_LA
            "ar", //    SCE_SYSTEM_PARAM_LANG_ARABIC
            "fr-ca" //  SCE_SYSTEM_PARAM_LANG_FRENCH_CA
        };

        int32_t ret = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_LANG, &language);
        if ((ret != SCE_OK) || (language >= numLangs))
        {
            return std::string();
        }

        return std::string(LocaleNames[language]);
    }

    void Locale::Initialize()
    {
    }

    void Locale::UnInitialize()
    {
    }
} /* namespace os */
} /* namespace il2cpp */
