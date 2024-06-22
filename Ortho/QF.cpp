#include "stdafx.h"
#include "QF.h"
#include <string>

// Application support //////////////////////////////////

static IApplicationPtr ptrApp = nullptr;	// QF.Application

// Helper function - prepare QF (resize, close all etc)
BOOL prepare_qf(WindowPtr& ptrWin)
{
	// Close all opened docs
	ptrApp->Models->Close();
	ptrApp->Problems->Close();

	//const std::string RandD{ "E:\\R&D\\" };		// devDell
	//const std::string RandD{ "E:\\R&D\\" };		// Dev10
	const std::string RandD{ "D:\\R&D\\" };			// SimonW540
	const std::string problemPath = RandD +
		//"������� ����. ��������\\2021-July_�������������\\_PART-2 (��-110 ������)\\Problems";
		//"������� ����. ��������\\2019-July-01_�������������\\��-750\\Problems";
		//"������� ����.��������\\2024-������\\Problems";
		"ImpulsProject\\2024-April\\Problems";

	QF()->DefaultFilePath = _bstr_t(problemPath.c_str());

	return TRUE;
}

// Return pointer to QF.Application ( initialize if necessary )
IApplicationPtr& QF()
{
	//todo?! support for situation when it was closed in the middle of session

	// Create if necessary
	if (!ptrApp) 
	{
		CoInitialize(nullptr);

		HRESULT hr = ptrApp.CreateInstance("Elcut.Application");
		if (FAILED(hr)) 
		{
			throw _com_error(hr);
		}

		// Set QF visible
		WindowPtr ptrWin = ptrApp->MainWindow;
		ptrWin->Visible = _variant_t(true);
		prepare_qf(ptrWin);
	}

	return ptrApp;
}

void ReleaseQF()
{
	if (ptrApp != NULL) {
		ptrApp.Release();
	}
}

void ResetQF()
{
	ptrApp = NULL;
}
