#include "stdafx.h"
#include "QF.h"

// Application support //////////////////////////////////

static IApplicationPtr ptrApp = nullptr;	// QF.Application

// Helper function - prepare QF (resize, close all etc)
BOOL prepare_qf(WindowPtr& ptrWin)
{
	// Close all opened docs
	ptrApp->Models->Close();
	ptrApp->Problems->Close();

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
