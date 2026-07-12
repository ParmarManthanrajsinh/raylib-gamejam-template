#include "save_data.h"
#include <raylib.h>
#include <string>
#include <cstring> // IWYU pragma: keep

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

t_SaveData LoadSaveData()
{
    t_SaveData data = {};

#if defined(PLATFORM_WEB)
    int completed = EM_ASM_INT
    ({
        var val = localStorage.getItem("hexgate_tutorial");
        return val === "1" ? 1 : 0;
    });
    data.tutorial_complete = (completed == 1);
#else
    std::string path = GetApplicationDirectory();
    path += "savedata.dat";

    if (FileExists(path.c_str()))
    {
        int bytesRead = 0;
        unsigned char* fileData = LoadFileData(path.c_str(), &bytesRead);
        if (fileData != nullptr)
        {
            if (bytesRead == sizeof(t_SaveData))
            {
                t_SaveData* saved = reinterpret_cast<t_SaveData*>(fileData);
                if (saved->magic == 0x48475854 && saved->version == 1)
                {
                    data = *saved;
                }
            }
            UnloadFileData(fileData);
        }
    }
#endif

    return data;
}

void SaveSaveData(const t_SaveData& data)
{
#if defined(PLATFORM_WEB)
    EM_ASM_({
        localStorage.setItem("hexgate_tutorial", $0 ? "1" : "0");
    }, data.tutorial_complete);
#else
    std::string path = GetApplicationDirectory();
    path += "savedata.dat";
    SaveFileData(path.c_str(), (void*)&data, sizeof(t_SaveData));
#endif
}
