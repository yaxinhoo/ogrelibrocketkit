/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#ifndef GKSEQUENCEPROCESS_H_
#define GKSEQUENCEPROCESS_H_

#include "Process/gkProcess.h"
#include "gkMathUtils.h"

class gkSequenceProcess : public gkProcess {

public:
	gkSequenceProcess(gkScalar maxTime=0);
	virtual ~gkSequenceProcess() {}

	void append(gkProcess* childProcess);
	void remove(gkProcess* childProcess);

	bool isFinished();
	void init();
	void update(gkScalar delta);

private:
	typedef utList<gkProcess*> ProcessList;

	int m_currentProcessNr;
	gkProcess* m_currentProcess;
	ProcessList m_initalProcessList,m_processList;
	gkScalar m_maxTime;
	gkScalar m_currentTime;
	bool m_isFinished;

};

#endif /* GKPARALLELPROCESS_H_ */
