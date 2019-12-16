#include "stdafx.h"
#include "QF.h"


QF::QF()
{
	// start ELCUT
	qf_ = nullptr;

	HRESULT hr = qf.CreateInstance("QuickField.Application");

}


QF::~QF()
{
}
