#pragma once

#import "d:\Fast\QuickField\origin_Dione\Common\TypelibEnglish\Debug\ActiveField.tlb" no_function_mapping 
// Remark: no_function_mapping is an undocumented directive that is neccessary
//   to prevent strange linker error (LNK2019 - unresolved reference). 
//   Apparently it happens if there are more then 1000 methods imported by #import directive
//   and if we access to these methods through template.
//   For more detais, see http://support.microsoft.com/kb/832688 and other
//   refereneces to 'no_function_mapping' in internet.

using namespace QuickField;

IApplicationPtr& QF();			// Return pointer to QF.Application ( initailize if necessary )
void ReleaseQF();				// Release QF.Application if it was allocated (kind of singleton destructor..)
void ResetQF();					// Reset QF (after if crashed ore something like this)

