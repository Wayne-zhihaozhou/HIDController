#include "InputSimulator.hpp"
using namespace Send;

#include "types.hpp"
#include "holder.hpp"
#include <IbSendBase.hpp>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        IbSendDestroy();
        break;
    }
    return TRUE;
}

