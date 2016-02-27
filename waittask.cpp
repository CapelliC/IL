/*
    IL : Prolog interpreter
    Copyright (C) 1992-2012 - Ing. Capelli Carlo

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

#include "stdafx.h"

bool WaitTaskExecution(UINT /*ntc*/)
{
#if 0
	CTimeSpan timeOut;
	CTime tStart = CTime::GetCurrentTime(), tCurr;

	timeOut = 20;
	while (GetNumTasks() == ntc)
	{
		if (!AfxGetApp()->PumpMessage())
			return FALSE;

		if (tStart + timeOut < CTime::GetCurrentTime())
		{
			TRACE("Timeout waiting for start\n");
			return FALSE;
		}
	}

	timeOut = 60 * 3;
	while (GetNumTasks() > ntc)
	{
		if (!AfxGetApp()->PumpMessage())
			return FALSE;

		if (tStart + timeOut < CTime::GetCurrentTime())
		{
			TRACE("Timeout waiting for end\n");
			return FALSE;
		}
	}

	return TRUE;
#else
	return FALSE;
#endif
}
