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
	static int	iCount;

	enum StatType {
		STR = 0, DEX, STA, CHA, WIS, INT, AGI,
		HP, Mana,
		SvFire, SvDisease, SvCold, SvMagic, SvPoison,
		Count = 14
	};

	char * GetRaceThreeLetterCode(int iRace)
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
public:
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
		sprintf_s(temp, "<c \"#000000\">%s<br>", Item->Name);
		strcat_s(out, temp);
		SetCXStr(&This->ItemInfo, out);
		memset(out, 0, MAX_STRING);

		// ITEM INFORMATION
		if (Item->Magic) {
			strcat_s(out, "MAGIC ITEM&nbsp;&nbsp;");
		}
		if (Item->Lore) {
			strcat_s(out, "LORE ITEM&nbsp;&nbsp;");
		}
		// why is it that 0 is prompting no drop, when 1 was supposed to prompt it?
		if (!Item->NoDrop) {
			strcat_s(out, "NO DROP&nbsp;&nbsp;");
		}
		/*if (Item->NoRent) {
			strcat(out, "NO RENT&nbsp;&nbsp;");
		}*/
		strcat_s(out, "<br>");

		// Skill // Attack Delay
		if (Item->ItemType < 6) { // see itemtypes.h for item type number values
			sprintf_s(temp, "Skill:&nbsp;%s&nbsp;&nbsp;&nbsp;", szItemTypes[Item->ItemType]);
			strcat_s(out, temp);
		}
		if (Item->Delay > 0) {
			sprintf_s(temp, "Atk Delay:&nbsp;%d<br>", Item->Delay);
			strcat_s(out, temp);
		}

		// Damage // AC
		if (Item->Damage > 0) {
			sprintf_s(temp, "DMG:&nbsp;%d&nbsp;&nbsp;&nbsp;", Item->Damage);
			strcat_s(out, temp);
		}
		if (Item->AC > 0) {
			sprintf_s(temp, "AC:&nbsp;%d", Item->AC);
			strcat_s(out, temp);
		}

		if (Item->Damage > 0 || Item->AC > 0) {
			strcat_s(out, "<br>");
		}

		SHORT Stats[14] = {
			Item->STR,		Item->DEX,			Item->STA,		Item->CHA,		Item->WIS,		Item->INT,		Item->AGI,
			Item->HP,		Item->Mana,
			Item->SvFire,	Item->SvDisease,	Item->SvCold,	Item->SvMagic,	Item->SvPoison
		};

		iCount = 0;
		for (int i = StatType::STR; i < StatType::Count; i++) {
			if (i + 1 == StatType::Count && iCount > 0) {
				strcat_s(out, "<br>");
				break;
			}

			SHORT CurrentStat = Stats[i];
			if (CurrentStat != 0) {
				iCount++;
				if (iCount % 5 == 0) {
					strcat_s(out, "<br>");
				}

				switch (i) {
					case StatType::STR:
						strcat_s(out, "STR:");
						break;
					case StatType::DEX:
						strcat_s(out, "DEX:");
						break;
					case StatType::STA:
						strcat_s(out, "STA:");
						break;
					case StatType::CHA:
						strcat_s(out, "CHA:");
						break;
					case StatType::WIS:
						strcat_s(out, "WIS:");
						break;
					case StatType::INT:
						strcat_s(out, "INT:");
						break;
					case StatType::AGI:
						strcat_s(out, "AGI:");
						break;
					case StatType::HP:
						strcat_s(out, "HP:");
						break;
					case StatType::Mana:
						strcat_s(out, "MANA:");
						break;
					case StatType::SvFire:
						strcat_s(out, "SV FIRE:");
						break;
					case StatType::SvDisease:
						strcat_s(out, "SV DISEASE:");
						break;
					case StatType::SvCold:
						strcat_s(out, "SV COLD:");
						break;
					case StatType::SvMagic:
						strcat_s(out, "SV MAGIC:");
						break;
					case StatType::SvPoison:
						strcat_s(out, "SV POISON:");
						break;
				}

				strcat_s(out, "&nbsp;");
				if (CurrentStat < 0) {
					sprintf_s(temp, "%d&nbsp;&nbsp;&nbsp;", CurrentStat);
				}
				else {
					sprintf_s(temp, "+%d&nbsp;&nbsp;&nbsp;", CurrentStat);
				}
				strcat(out, temp);
			}
		}

		AppendCXStr(&This->ItemInfo, out);
		memset(out, 0, MAX_STRING);

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

		// commenting out scroll for now since the scroll name already shows.
		/*if (Item->Scroll.SpellID > 0 && Item->Scroll.SpellID != -1) {
			SetSpell_Detour(Item->Scroll.SpellID, false, 0);
		}*/
		bNoSpellTramp = false;

		// Weight
		float weight = Item->Weight * 0.1f;
		sprintf_s(temp, "WT:&nbsp;&nbsp;%.1f<br>", weight);
		strcat_s(out, temp);

		// Classes
		int iClass = 1;
		iCount = 0;
		if ((int)(1 << TotalClasses) - 1 != Item->Classes && Item->Classes != 0) {
			char cClasses[MAX_STRING] = { 0 };
			for (WORD i = Warrior; i < TotalClasses + 1; i++) {
				if (Item->Classes & iClass) {
					iCount++;
					char * cCode = pEverQuest->GetClassThreeLetterCode(i);
					if (cCode == NULL) {
						break;
					}

					if (iCount % 7 == 0) {
						strcat_s(cClasses, "<br>");
						strcat_s(cClasses, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
					}
					strcat_s(cClasses, cCode);
					strcat_s(cClasses, " ");
				}
				iClass <<= 1;
			}
			sprintf_s(temp, "Class:&nbsp;%s<br>", cClasses);
			strcat_s(out, temp);
		}
		else {
			strcat_s(out, "Class:&nbsp;ALL<br>");
		}

		// Races
		int iRace = 1;
		iCount = 0;
		if ((int)(1 << 16) - 1 != Item->Races && Item->Races != 0) {
			char Races[MAX_STRING] = { 0 };
			for (WORD i = 1; i < 16; i++) {
				if (Item->Races & iRace) {
					iCount++;
					char * race = GetRaceThreeLetterCode(i);
					if (race == NULL) {
						break;
					}

					if (iCount % 7 == 0) {
						strcat_s(Races, "<br>");
						strcat_s(Races, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
					}
					strcat_s(Races, race);
					strcat_s(Races, " ");
				}
				iRace <<= 1;
			}
			sprintf_s(temp, "Race:&nbsp;&nbsp;%s<br>", Races);
			strcat_s(out, temp);
		}
		else {
			strcat_s(out, "Race:&nbsp;&nbsp;ALL<br>");
		}

		// final tag to add color
		strcat_s(out, "</c>");
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
			sprintf_s(temp, "Effect:&nbsp;%s<br>", pSpell->Name);
			strcat_s(out, temp);
		}

		AppendCXStr(&This->ItemInfo, out);
	}
};

bool ItemDisplayHook::bNoSpellTramp = false;
int ItemDisplayHook::iCount = 0;

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
	// EasyClassDetour(CItemDisplayWnd__SetSpell,ItemDisplayHook,SetSpell_Detour,void,(int SpellID,bool HasSpellDescr,int),SetSpell_Trampoline);
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
