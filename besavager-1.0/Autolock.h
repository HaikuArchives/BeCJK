// BeSavager - Savager Workroom Development Toolkit for BeOS
// (C) Copyright by Anthony Lee <anthonylee@eLong.com> 2002
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
//
//
// Autolock like BAutolock but to be template

#ifndef __SAVAGER_AUTOLOCK_H__
#define __SAVAGER_AUTOLOCK_H__

template<class C>
class SAutolock
{
public:
	SAutolock(C *target)
	{
		locker_owner = target;
		islocked = (locker_owner != NULL) ? locker_owner->Lock() : false;
	}

	SAutolock(C &target)
	{
		locker_owner = &target;
		islocked = locker_owner->Lock();
	}

	~SAutolock()
	{
		if(islocked && locker_owner) locker_owner->Unlock();
	}
	

	bool IsLocked() const
	{
		return islocked;
	}

	C* Target() const
	{
		return locker_owner;
	}


private:
	bool islocked;
	C* locker_owner;
};

#endif /* __SAVAGER_AUTOLOCK_H__ */

