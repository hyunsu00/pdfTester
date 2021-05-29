// fpdf_raii.h
#pragma once
#include <memory> // std::unique_ptr
#include <type_traits> // std::remove_pointer
#include <fpdfview.h> // FPDF_CloseDocument, FPDF_ClosePage, FPDFText_ClosePage, FPDFBitmap_Destroy
#include <fpdf_formfill.h> // FPDFDOC_ExitFormFillEnvironment
#include <fpdf_text.h> // FPDFText_ClosePage
#include <cpp/fpdf_scopers.h> // ScopedFPDFDocument, ScopedFPDFFormHandle, ScopedFPDFPage, 
							  // ScopedFPDFTextPage, ScopedFPDFBitmap

namespace {
	using AutoFPDFDocumentPtr = ScopedFPDFDocument;
	using AutoFPDFFormHandlePtr = ScopedFPDFFormHandle;
	using AutoFPDFPagePtr = ScopedFPDFPage;
	using AutoFPDFTextPagePtr = ScopedFPDFTextPage;
	using AutoFPDFBitmapPtr = ScopedFPDFBitmap;
}
