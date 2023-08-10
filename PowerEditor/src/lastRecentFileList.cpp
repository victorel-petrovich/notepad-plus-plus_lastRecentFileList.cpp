// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.



#include "lastRecentFileList.h"
#include "menuCmdID.h"
#include "localization.h"

//me added for debug prints; but can use printf() wihtou including anything here. 
// #include <iostream>
// using std::cout;

/*debug
*/

// not callers & not callees (in this file)

/*
  initMenu(..), of class LastRecentFileList, is only called once in src folder: in Notepad_plus.cpp, inside init(...), line 580:
 _lastRecentFileList.initMenu(hFileMenu, IDM_FILEMENU_LASTONE + 1, IDM_FILEMENU_EXISTCMDPOSITION, &_accelerator, nppParam.putRecentFileInSubMenu());
	and is followed by calls to add() (from this file) to add RFH file items to menu 
	
 see  menuCmdID.h  
 IDM_FILE_EXIT=41011 (the id) 
 * at line: 
	// 0 based position of command "Exit"
 * IDM_FILEMENU_EXISTCMDPOSITION = 22
 also see what what positions, relative to 0 at top, should be when _lrfl is empty (_lrfl is the internal list of recent files)
*/
void LastRecentFileList::initMenu(HMENU hMenu, int idBase, int posBase, Accelerator *pAccelerator, bool doSubMenu)
{
	//me added for debug prints
	AllocConsole();
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);
	//-----------------------------

	printf("initMenu()\n\n" );
	
	if (doSubMenu)
	{
		_hParentMenu = hMenu;
		_hMenu = ::CreatePopupMenu();
	}
	else
	{
		_hParentMenu = NULL;
		_hMenu = hMenu;
	}

	_idBase = idBase;
	_posBase = posBase;
	_pAccelerator = pAccelerator;
	_nativeLangEncoding = NPP_CP_WIN_1252;

	for (size_t i = 0 ; i < sizeof(_idFreeArray) ; ++i)
		_idFreeArray[i] = true;
	
	/*debugs
	*/
	printf("\t _size=%d\n", _size );
	printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
	printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );	
}

/*
switchMode is only called once in scr folder: in NppBigSwitch.cpp
call to switchMode() is followed by call to updateMenu(), defined lower ; 
calls done at: case NPPM_INTERNAL_RECENTFILELIST_SWITCH

TODO: call updateMenu() inside switchMode(), at end, and remove call in NppBigSwitch.cpp, 
	to be same as with the other 4 functions in current file. 
*/
void LastRecentFileList::switchMode()
{
	printf("switchMode()\n\n");
	
	//Remove all recent file history menu items that are commands (including recent files )
	::RemoveMenu(_hMenu, IDM_FILE_RESTORELASTCLOSEDFILE, MF_BYCOMMAND);
	::RemoveMenu(_hMenu, IDM_OPEN_ALL_RECENT_FILE, MF_BYCOMMAND);
	::RemoveMenu(_hMenu, IDM_CLEAN_RECENT_FILE_LIST, MF_BYCOMMAND);

	for (int i = 0; i < _size; ++i)
	{
		::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
	}

	if (_hParentMenu == NULL) // mode main menu (recent files - also in main-menu); thus _hMenu points to main-menu
	{	
		/*
		// If _lrfl was empty, then in main-menu after "print now", should have:		
		-----------------			_posBase-1
		Exit						_posBase
		
		// If _lrfl was not empty, then have:
		-----------------			_posBase-1
		-----------------			_posBase
		-----------------			_posBase+1
		Exit						_posBase+2
		
		*/
		/*debug
		printf("switchMode(),if (_hParentMenu == NULL): \n" );
		printf("\t _size=%d\n", _size );
		printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
		printf("\t GetMenuItemID(_hMenu, _posBase-1)=%u\n", GetMenuItemID(_hMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+0)=%u\n", GetMenuItemID(_hMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hMenu, _posBase+1)=%u\n", GetMenuItemID(_hMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+2)=%u\n", GetMenuItemID(_hMenu, _posBase+2) );
		printf("\n");
		*/
		if (_size > 0) // remove 2 extra "bars", as the "----" are called in menuCmdID.h 
		{
			// the first removal below makes the next bar take position _posBase
			::RemoveMenu(_hMenu, _posBase, MF_BYPOSITION);
			::RemoveMenu(_hMenu, _posBase, MF_BYPOSITION);
		}		
		// switch to sub-menu mode
		_hParentMenu = _hMenu;
		_hMenu = ::CreatePopupMenu(); //  in updateMenu(), this _hMenu will be attached to the hParentMenu(main-menu) at _posBase, and populated
		/*debug
		printf("switchMode(),if (_hParentMenu == NULL), end BEFORE last line: \n" );
		printf("\t _size=%d\n", _size );
		printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
		printf("\t GetMenuItemID(_hMenu, _posBase-1)=%u\n", GetMenuItemID(_hMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+0)=%u\n", GetMenuItemID(_hMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hMenu, _posBase+1)=%u\n", GetMenuItemID(_hMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+2)=%u\n", GetMenuItemID(_hMenu, _posBase+2) );
		printf("\n");
		*/
		
		// ::RemoveMenu(_hMenu, _posBase+1, MF_BYPOSITION);  //  redundant line, tested . The new menu created by CreatePopupMenu() is already empty. 
		/*debug
		printf("switchMode(),if (_hParentMenu == NULL), end AFTER last line: \n\n" );
		printf("\t _size=%d\n", _size );
		printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
		printf("\t GetMenuItemID(_hMenu, _posBase-1)=%u\n", GetMenuItemID(_hMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+0)=%u\n", GetMenuItemID(_hMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hMenu, _posBase+1)=%u\n", GetMenuItemID(_hMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+2)=%u\n", GetMenuItemID(_hMenu, _posBase+2) );
		printf("\n");
		printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%u\n", GetMenuItemID(_hParentMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%u\n", GetMenuItemID(_hParentMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%u\n", GetMenuItemID(_hParentMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%u\n", GetMenuItemID(_hParentMenu, _posBase+2) );
		*/
		
	}
	else // mode sub-menu ; _hMenu points to sub-menu; _hParentMenu points to main-menu 
	{
		/*
		// If _lrfl was empty, then in main-menu after "print now", should have:		
		-----------------			_posBase-1
		Exit						_posBase
		
		// If _lrfl was not empty, then should have:
		-----------------			_posBase-1
		RecentFiles ->				_posBase
		-----------------			_posBase+1
		Exit						_posBase+2
		
		*/

		
		/*debug
		printf("switchMode(), else: \n\n");
		printf("\t _size=%d\n", _size );
		printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%u\n", GetMenuItemID(_hParentMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%u\n", GetMenuItemID(_hParentMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%u\n", GetMenuItemID(_hParentMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%u\n", GetMenuItemID(_hParentMenu, _posBase+2) );
		printf("\n");
		printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
		printf("\t GetMenuItemID(_hMenu, _posBase-1)=%u\n", GetMenuItemID(_hMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+0)=%u\n", GetMenuItemID(_hMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hMenu, _posBase+1)=%u\n", GetMenuItemID(_hMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+2)=%u\n", GetMenuItemID(_hMenu, _posBase+2) );
		*/
		
		if (_size > 0)//remove "RecentFiles ->" and 1 extra bar
		{
			::RemoveMenu(_hParentMenu, _posBase, MF_BYPOSITION);
			::RemoveMenu(_hParentMenu, _posBase, MF_BYPOSITION);
		}
		// switch to main menu mode
		::DestroyMenu(_hMenu);
		_hMenu = _hParentMenu;
		_hParentMenu = NULL;

		/*debug
		printf("switchMode(), else, at end: \n");
		printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
		printf("\t GetMenuItemID(_hMenu, _posBase-1)=%u\n", GetMenuItemID(_hMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+0)=%u\n", GetMenuItemID(_hMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hMenu, _posBase+1)=%u\n", GetMenuItemID(_hMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hMenu, _posBase+2)=%u\n", GetMenuItemID(_hMenu, _posBase+2) );
		*/
		
	}
	_hasSeparators = false; // I think by "separators" it's meant whatever extra menu items + bars between: the bar after Print-now, and Exit.
	/*
	Now in main-menu after "Print now", have:		
	-----------------			_posBase-1
	Exit						_posBase
	*/
	
}

/* 
called only in: NppBigSwitch.cpp, at: 
case WM_CLOSE:
*/
void LastRecentFileList::saveLRFL()
{
	NppParameters& nppParams = NppParameters::getInstance();
	if (nppParams.writeRecentFileHistorySettings(_userMax))
	{
		for (int i = _size - 1; i >= 0; i--)	//reverse order: so loading goes in correct order
		{
			nppParams.writeHistory(_lrfl.at(i)._name.c_str());
		}
	}
}


//callers but not callees

/*
 add(..) called in:
- Notepad_plus.cpp , right after initMenu(..), to add RFH files to menu 
	Line  587
- NppIO.cpp:
	- at closing a doc(tab): Line  830
	- in fileSaveAs(..) Line 1812
	
*/
void LastRecentFileList::add(const TCHAR *fn) 
{
	printf("add()\n\n" );
	
	if (_userMax == 0 || _locked)
		return;

	RecentItem itemToAdd(fn);

	int index = find(fn);
	if (index != -1)
	{
		//already in list, bump upwards
		remove(index);
	}

	if (_size == _userMax)
	{
		itemToAdd._id = _lrfl.back()._id;
		_lrfl.pop_back();	//remove oldest
	}
	else
	{
		itemToAdd._id = popFirstAvailableID();
		++_size;
	}
	_lrfl.push_front(itemToAdd);
	updateMenu();
	/*debugs
	*/
	printf("add() at end: \n\n" );	
	printf("\t _size=%d\n", _size );
	printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
	printf("\t GetMenuItemID(_hMenu, _posBase-1)=%u\n", GetMenuItemID(_hMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hMenu, _posBase+0)=%u\n", GetMenuItemID(_hMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hMenu, _posBase+1)=%u\n", GetMenuItemID(_hMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hMenu, _posBase+2)=%u\n", GetMenuItemID(_hMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hMenu, _posBase+3)=%u\n", GetMenuItemID(_hMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hMenu, _posBase+4)=%u\n", GetMenuItemID(_hMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hMenu, _posBase+5)=%u\n", GetMenuItemID(_hMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hMenu, _posBase+6)=%u\n", GetMenuItemID(_hMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hMenu, _posBase+7)=%u\n", GetMenuItemID(_hMenu, _posBase+7) );
	printf("\t GetMenuItemID(_hMenu, _posBase+8)=%u\n", GetMenuItemID(_hMenu, _posBase+8) );
	printf("\n");
	printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%u\n", GetMenuItemID(_hParentMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%u\n", GetMenuItemID(_hParentMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%u\n", GetMenuItemID(_hParentMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%u\n", GetMenuItemID(_hParentMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+3)=%u\n", GetMenuItemID(_hParentMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+4)=%u\n", GetMenuItemID(_hParentMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+5)=%u\n", GetMenuItemID(_hParentMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+6)=%u\n", GetMenuItemID(_hParentMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+7)=%u\n", GetMenuItemID(_hParentMenu, _posBase+7) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+8)=%u\n", GetMenuItemID(_hParentMenu, _posBase+8) );
	
}

void LastRecentFileList::remove(const TCHAR *fn) 
{ 
	int index = find(fn);
	if (index != -1)
		remove(index);
}

/*
called in NppCommands.cpp at:
case IDM_CLEAN_RECENT_FILE_LIST :
*/
void LastRecentFileList::clear() 
{
	printf("clear()\n\n" );
	
	if (_size == 0)
		return;

	for (int i = (_size-1); i >= 0; i--) 
	{
		::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
		setAvailable(_lrfl.at(i)._id);
		_lrfl.erase(_lrfl.begin() + i);
	}
	_size = 0;
	updateMenu();
}

/* 
called only in: NppBigSwitch.cpp, at: 
case NPPM_INTERNAL_SETTING_HISTORY_SIZE:
*/
void LastRecentFileList::setUserMaxNbLRF(int size)
{
	_userMax = size;
	if (_size > _userMax) 
	{	//start popping items
		int toPop = _size-_userMax;
		while (toPop > 0) 
		{
			::RemoveMenu(_hMenu, _lrfl.back()._id, MF_BYCOMMAND);
			setAvailable(_lrfl.back()._id);
			_lrfl.pop_back();
			toPop--;
			_size--;
		}
		updateMenu();
		_size = _userMax;
	}
}

//callers and callees 

/*
called by remove(fn) above, and by add(fn) above
*/
void LastRecentFileList::remove(size_t index) 
{
	printf("remove(size_t index)\n\n" );
	
	if (_size == 0 || _locked)
		return;
	if (index < _lrfl.size())
	{
		::RemoveMenu(_hMenu, _lrfl.at(index)._id, MF_BYCOMMAND);
		setAvailable(_lrfl.at(index)._id);
		_lrfl.erase(_lrfl.begin() + index);
		--_size;
		updateMenu();
	}
}


//not callers but callees (all the rest )

/*
updateMenu() has 3 main functions:
- add missing LRFL menu items (when _size>0) 
- remove unnecessary LRFL menu items (when _size==0)
- update updates the menu filenames based on current _lrfl

updateMenu() is called in 2 files only in src:
- in NppBigSwitch.cpp in process(...), switch(message):
	case NPPM_INTERNAL_RECENTFILELIST_UPDATE:
		call to updateMenu() alone
	case NPPM_INTERNAL_RECENTFILELIST_SWITCH:
		call to switchMode() is followed by call to updateMenu();  
- in current file, in 4 places:
	LastRecentFileList::add(const TCHAR *fn) 
	LastRecentFileList::remove(size_t index) 
	LastRecentFileList::clear()
	LastRecentFileList::setUserMaxNbLRF(int size)
*/
void LastRecentFileList::updateMenu()
{
		/*debug
		*/
	printf("updateMenu()\n\n" );

	NppParameters& nppParam = NppParameters::getInstance();
	if (!_hasSeparators && _size > 0)  // add missing RFH menu items: in main-menu, and, if submenu mode, also in sub-menu
	{	
		//add separators in the main-menu
		
		// preparation of the names for 4 separators items 
		NativeLangSpeaker *pNativeLangSpeaker = nppParam.getNativeLangSpeaker();

		generic_string recentFileList = pNativeLangSpeaker->getSubMenuEntryName("file-recentFiles");
		generic_string openRecentClosedFile = pNativeLangSpeaker->getNativeLangMenuString(IDM_FILE_RESTORELASTCLOSEDFILE);
		generic_string openAllFiles = pNativeLangSpeaker->getNativeLangMenuString(IDM_OPEN_ALL_RECENT_FILE);
		generic_string cleanFileList = pNativeLangSpeaker->getNativeLangMenuString(IDM_CLEAN_RECENT_FILE_LIST);

		if (recentFileList == TEXT(""))
			recentFileList = TEXT("&Recent Files");
		if (openRecentClosedFile == TEXT(""))
			openRecentClosedFile = TEXT("Restore Recent Closed File");
		if (openAllFiles == TEXT(""))
			openAllFiles = TEXT("Open All Recent Files");
		if (cleanFileList == TEXT(""))
			cleanFileList = TEXT("Empty Recent Files List");

		int deltaPosRRCF; // my add; for testing diff values in sub-menu vs main-menu  so that (_posBase+deltaPosRRCF) will be position where will insert RRCF, "Restore Recent Closed File"
		
		if (!isSubMenuMode())
		{
			// printf("updateMenu(), if (!isSubMenuMode()), BEFORE insert bar, GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
			::InsertMenu(_hMenu, _posBase + 0, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0); // a bar
			// printf("updateMenu(), if (!isSubMenuMode()), AFTER insert bar, GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
			
			/*
			 Now in main-menu after "print now", have:		
			 -----------------				_posBase-1
			 -----------------				_posBase
			 Exit							_posBase+1		
			*/
			deltaPosRRCF=1; 
		}
		else 
		{
			// printf("updateMenu(), if ( isSubMenuMode()), BEFORE insert \"Recent Files\" and bar, GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
			
			::InsertMenu(_hParentMenu, _posBase + 0, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR>(_hMenu), (LPCTSTR)recentFileList.c_str());
			::InsertMenu(_hParentMenu, _posBase + 1, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0);
			
			// printf("updateMenu(), if (!isSubMenuMode()), AFTER insert \"Recent Files\" and bar, GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
			/*
			 Now in main-menu after "print now", have:		
			 -----------------				_posBase-1
			 RecentFiles ->					_posBase
			 -----------------				_posBase+1
			 Exit							_posBase+2					
			*/
			deltaPosRRCF=1;
		}
		// Now for both modes:
		/*
			`+deltaPosRRCF` below is my add, as well as changing sequence of additions from 1...4, to 0..3
		*/
		::InsertMenu(_hMenu, _posBase + 0+deltaPosRRCF, MF_BYPOSITION, IDM_FILE_RESTORELASTCLOSEDFILE, openRecentClosedFile.c_str());
		::InsertMenu(_hMenu, _posBase + 1+deltaPosRRCF, MF_BYPOSITION, IDM_OPEN_ALL_RECENT_FILE, openAllFiles.c_str());
		::InsertMenu(_hMenu, _posBase + 2+deltaPosRRCF, MF_BYPOSITION, IDM_CLEAN_RECENT_FILE_LIST, cleanFileList.c_str());
		::InsertMenu(_hMenu, _posBase + 3+deltaPosRRCF, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0);

		_hasSeparators = true;
	}
	else if (_hasSeparators && _size == 0) 	//remove separators 
	// ex, after clear(), and possibly after remove(), add(), setUserMaxNbLRF()
	{
		/*
		//If no submenu: 
		 
		*In main-menu after "print now", have:		
		 -----------------				_posBase-1
		 -----------------				_posBase
		 RRCF							_posBase+1
		 OARF							_posBase+2
		 ERFL							_posBase+3
		 -----------------				_posBase+4
		 Exit							_posBase+5					
		 
		//If submenu: 
		 
		*In main-menu after "print now", have:		
		 -----------------				_posBase-1
		 RecentFiles ->					_posBase
		 -----------------				_posBase+1
		 Exit							_posBase+2					
		 
		*In sub-menu have:               ??????????need verify below numbers  
		 RRCF							_posBase+1
		 OARF							_posBase+2
		 ERFL							_posBase+3
		 -----------------				_posBase+4		 
		*/		
		::RemoveMenu(_hMenu, _posBase + 4, MF_BYPOSITION);//  bar 3 in main-menu; nothing in sub-menu
		::RemoveMenu(_hMenu, IDM_CLEAN_RECENT_FILE_LIST, MF_BYCOMMAND);
		::RemoveMenu(_hMenu, IDM_OPEN_ALL_RECENT_FILE, MF_BYCOMMAND);
		::RemoveMenu(_hMenu, IDM_FILE_RESTORELASTCLOSEDFILE, MF_BYCOMMAND);
		::RemoveMenu(_hMenu, _posBase + 0, MF_BYPOSITION); // bar 2 in main-menu ; bar 1 (the only) in sub-menu
		_hasSeparators = false;

		if (isSubMenuMode())
		{
			// Remove bar 2 and "Recent Files" Entry from the main-menu
			::RemoveMenu(_hParentMenu, _posBase + 1, MF_BYPOSITION);
			::RemoveMenu(_hParentMenu, _posBase + 0, MF_BYPOSITION);

			// Remove the last left bar from the sub-menu
			::RemoveMenu(_hMenu, 0, MF_BYPOSITION); // ???????????? there is no left bar there 
		}
	}

	/* Note X
	At this point, if _size>0, then in sub-menu, have these items:
	RRCF			_posBase
	OARF			_posBase+1
	ERFL			_posBase+2
	-------			_posBase+3			
	1: filename1	_posBase+4
	2: filename2	_posBase+5
	....
	Proof: 
	if _size>0, then it must be that _hasSeparators==true (if it were false, then 1st branch of top-level if above would have made it true ). 
	But, the only way for _hasSeparators to become true is as a result of that mentioned branch , because it is initialized in initMenu() as false, and there is _no other place_ in this file that changes it to true. 
	
	*/

	_pAccelerator->updateFullMenu();

	//Remove all menu recentFiles items
	for (int i = 0; i < _size; ++i) 
	{
		::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
	}
	//Then read them, so everything stays in sync
	for (int j = 0; j < _size; ++j)
	{
		generic_string strBuffer(BuildMenuFileName(nppParam.getRecentFileCustomLength(), j, _lrfl.at(j)._name) );
		::InsertMenu(_hMenu, _posBase + j, MF_BYPOSITION, _lrfl.at(j)._id, strBuffer.c_str());
		/*??
		In case of sub-menu, how/why (_posBase+j) results in insertion at position after the 2 items: "empty recent files list" and bar, instead of at the top, because (_posBase) is position at the top of the sub-menu ? 
		
		According to Note X above , before this for-loop, if _size>0 ( and unless _pAccelerator->updateFullMenu(); does some tricks), in sub-menu have already next 4 items at top: (below I use abbreviations of the menu items)
			RRCF			_posBase
			OARF			_posBase+1
			ERFL			_posBase+2
			-------			_posBase+3
			
		If it was intended to placce after the last 2 items, then must have use (posBase+4+j). 
		*/
	}
	
}

/*
 get filename w/ given id; if no file has such id, return first filename 
*/
generic_string & LastRecentFileList::getItem(int id) 
{
	int i = 0;
	for (; i < _size; ++i)
	{
		if (_lrfl.at(i)._id == id)
			break;
	}
	if (i == _size)
		i = 0;
	return _lrfl.at(i)._name;	//if id not found, return first
}

/*
 get name of file at given index; 
*/
generic_string & LastRecentFileList::getIndex(int index)
{
	return _lrfl.at(index)._name;	//if not found, return first // ?? I think it's wrong comment here .
}

//private methods

/*
get index of given filename; if not found => -1
*/
int LastRecentFileList::find(const TCHAR *fn)
{
	for (int i = 0; i < _size; ++i)
	{
		if (OrdinalIgnoreCaseCompareStrings(_lrfl.at(i)._name.c_str(), fn) == 0)
		{
			return i;
		}
	}
	return -1;
}

int LastRecentFileList::popFirstAvailableID() 
{
	for (int i = 0 ; i < NB_MAX_LRF_FILE ; ++i)
	{
		if (_idFreeArray[i])
		{
			_idFreeArray[i] = false;
			return i + _idBase;
		}
	}
	return 0;
}

void LastRecentFileList::setAvailable(int id)
{
	int index = id - _idBase;
	_idFreeArray[index] = true;
}
