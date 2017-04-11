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
	static bool bNoSpellTramp;
public:
	const char * GetRaceThreeLetterCode(int iRace)
	{
		switch (iRace) {
		case 1: return ("HUM");
		case 2: return ("BAR");
		case 3: return ("ERU");
		case 4: return ("ELF");
		case 5: return ("HIE");
		case 6: return ("DEF");
		case 7: return ("HEF");
		case 8: return ("DWF");
		case 9: return ("TRL");
		case 10:return ("OGR");
		case 11:return ("HLF");
		case 12:return ("GNM");
		case 13:return ("IKS");
		case 14:return ("VAH");
		case 15:return ("FRG");
		}

		return ("UNKNOWN RACE");
	}

	VOID CleanOutput(char * cOutput) {
		memset(cOutput, 0, MAX_STRING);
	}

	VOID SetItem_Trampoline(class EQ_Item *pitem,bool unknown);
	VOID SetItem_Detour(class EQ_Item *pitem,bool unknown)
	{
		PEQITEMWINDOW This=(PEQITEMWINDOW)this;
		PCONTENTS item=(PCONTENTS)pitem;
		PITEMINFO Item=(PITEMINFO)item->Item;
		CHAR out[MAX_STRING] = { 0 };
		CHAR temp[MAX_STRING] = { 0 };
		SetItem_Trampoline(pitem,unknown);

		// add the name to the front of the item info and turn it black
		sprintf(temp, "<c \"#000000\">%s<br>", Item->Name);
		strcat(out, temp);
		SetCXStr(&This->ItemInfo, out);
		CleanOutput(out);

		// ITEM INFORMATION
		if (Item->Magic) {
			strcat(out, "MAGIC ITEM&nbsp;&nbsp;");
		}
		if (Item->Lore) {
			strcat(out, "LORE ITEM&nbsp;&nbsp;");
		}
		// why is it that 0 is prompting no drop, when 1 was supposed to prompt it?
		if (!Item->NoDrop) {
			strcat(out, "NO DROP&nbsp;&nbsp;");
		}
		/*if (Item->NoRent) {
			strcat(out, "NO RENT&nbsp;&nbsp;");
		}*/
		strcat(out, "<br>");

		// AC
		if (Item->AC > 0) {
			sprintf(temp, "AC:&nbsp;%d<br>", Item->AC);
			strcat(out, temp);
		}

		AppendCXStr(&This->ItemInfo, out);
		CleanOutput(out);

		// Spell info
		bNoSpellTramp = true;
		if (Item->Clicky.SpellID > 0 && Item->Clicky.SpellID != -1) {
			SetSpell_Detour(Item->Clicky.SpellID, false, 0);
		}
		if (Item->Proc.SpellID > 0 && Item->Proc.SpellID != -1) {
			SetSpell_Detour(Item->Proc.SpellID, false, 0);
		}
		if (Item->Worn.SpellID > 0 && Item->Worn.SpellID != -1) {
			SetSpell_Detour(Item->Worn.SpellID, false, 0);
		}
		if (Item->Scroll.SpellID > 0 && Item->Scroll.SpellID != -1) {
			SetSpell_Detour(Item->Scroll.SpellID, false, 0);
		}
		bNoSpellTramp = false;

		// Weight
		float weight = Item->Weight * 0.1f;
		sprintf(temp, "WT:&nbsp;&nbsp;%.1f<br>", weight);
		strcat(out, temp);

		// Classes
		int iClassBit = 1 << (GetCharInfo2()->Class - 1);
		//if (!(Item->Classes & iClassBit)) {
			char cClasses[64] = { 0 };
			int iClass = 1;
			for (WORD i = 1; i < 16; i++) {
				if (Item->Classes & iClass) {
					char * cCode = pEverQuest->GetClassThreeLetterCode(i);
					if (cCode == NULL) {
						break;
					}

					strcat(cClasses, cCode);
					strcat(cClasses, " ");
				}
				iClass *= 2;
			}

			sprintf(temp, "Class:&nbsp;%s<br>", cClasses);
			strcat(out, temp);
		//}
		/*else {
			char * cCode = pEverQuest->GetClassThreeLetterCode(GetCharInfo2()->Class);
			sprintf(temp, "Class:&nbsp;&nbsp;ALL<br>");
			strcat(out, temp);
		}*/

		// Races

		// final tag to add color
		strcat(out, "</c>");
		AppendCXStr(&This->ItemInfo, out);
	}

	VOID SetSpell_Trampoline(int SpellID, bool HasSpellDescr, int unknown_int);
	VOID SetSpell_Detour(int SpellID, bool HasSpellDescr, int unknown_int)
	{
		PEQITEMWINDOW This = (PEQITEMWINDOW)this;
		PCHARINFO pCharInfo = NULL;
		if (NULL == (pCharInfo = GetCharInfo())) return;
		PSPELL pSpell = GetSpellByID(SpellID);
		if (pSpell == NULL) {
			return;
		}

		CHAR out[MAX_STRING] = { 0 };
		CHAR temp[MAX_STRING] = { 0 };
		if (!bNoSpellTramp) {
			SetSpell_Trampoline(SpellID, HasSpellDescr, unknown_int);
		}
		else {
			sprintf(temp, "Effect:&nbsp;%s<br>", pSpell->Name);
			strcat(out, temp);
		}

		AppendCXStr(&This->ItemInfo, out);
	}
};

bool ItemDisplayHook::bNoSpellTramp = false;

DETOUR_TRAMPOLINE_EMPTY(VOID ItemDisplayHook::SetItem_Trampoline(class EQ_Item *,bool));
DETOUR_TRAMPOLINE_EMPTY(VOID ItemDisplayHook::SetSpell_Trampoline(int SpellID, bool HasSpellDescr, int));

#ifndef ISXEQ
// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2ItemDisplay");

	// Add commands, macro parameters, hooks, etc.

	// EasyClassDetour(CItemDisplayWnd__SetItem,ItemDisplayHook,SetItem_Detour,void,(class EQ_Item *, bool),SetItem_Trampoline);
	EzDetour(CItemDisplayWnd__SetItem,ItemDisplayHook::SetItem_Detour,ItemDisplayHook::SetItem_Trampoline);
	//   EasyClassDetour(CItemDisplayWnd__SetSpell,ItemDisplayHook,SetSpell_Detour,void,(int SpellID,bool HasSpellDescr,int),SetSpell_Trampoline);
	EzDetour(CItemDisplayWnd__SetSpell, ItemDisplayHook::SetSpell_Detour, ItemDisplayHook::SetSpell_Trampoline);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2ItemDisplay");

	// Remove commands, macro parameters, hooks, etc.
	RemoveDetour(CItemDisplayWnd__SetItem);
	RemoveDetour(CItemDisplayWnd__SetSpell);
}
#endif
