// MQ2ItemDisplay.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.


#ifndef ISXEQ
#include "../MQ2Plugin.h"
PreSetup("MQ2ItemDisplay");
#else
#include "../ISXEQClient.h"
#include "ISXEQItemDisplay.h"
#endif

// *************************************************************************** 
// Function:    ItemDisplayHook
// Description: Our Item display hook 
// *************************************************************************** 
class ItemDisplayHook
{
public:
	bool CXStrReplace (PCXSTR * Str, const char * cFind, const char * cReplace)
	{
		char cTemp[2048];
		DWORD dwSize = GetCXStr (*Str, cTemp, sizeof (cTemp));
		if (dwSize > 0 && dwSize < sizeof (cTemp) - 20) {
			char * cPtr = strstr (cTemp, cFind);
			if (cPtr != NULL) {
				char * cDup = strdup (cPtr);

				strcpy (cPtr, cReplace);
				strcpy (cPtr + strlen (cReplace), cDup + strlen (cFind));

				free (cDup);

				SetCXStr (Str, cTemp);
				return true;
			}
		}
		return false;
	}

	VOID SetItem_Trampoline(class EQ_Item *pitem,bool unknown);
	VOID SetItem_Detour(class EQ_Item *pitem,bool unknown)
	{
		PEQITEMWINDOW This=(PEQITEMWINDOW)this;
		PCONTENTS item=(PCONTENTS)pitem;
		PITEMINFO Item=(PITEMINFO)item->Item;
		CHAR out[MAX_STRING] = { 0 };
		CHAR data[MAX_STRING] = { 0 };
		CHAR temp[MAX_STRING] = { 0 };
		SetItem_Trampoline(pitem,unknown);

		sprintf(temp, "%s<BR>", Item->Name);
		strcat(out, temp);
		GetCXStr(This->ItemInfo, data, MAX_STRING);
		strcat(out, data);

		// append it all to our item info
		SetCXStr(&This->ItemInfo, out);
	}
};

DETOUR_TRAMPOLINE_EMPTY(VOID ItemDisplayHook::SetItem_Trampoline(class EQ_Item *,bool)); 

#ifndef ISXEQ
// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2ItemDisplay");

	// Add commands, macro parameters, hooks, etc.

	// EasyClassDetour(CItemDisplayWnd__SetItem,ItemDisplayHook,SetItem_Detour,void,(class EQ_Item *, bool),SetItem_Trampoline);
	EzDetour(CItemDisplayWnd__SetItem,ItemDisplayHook::SetItem_Detour,ItemDisplayHook::SetItem_Trampoline);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2ItemDisplay");

	// Remove commands, macro parameters, hooks, etc.
	RemoveDetour(CItemDisplayWnd__SetItem);
}
#endif
