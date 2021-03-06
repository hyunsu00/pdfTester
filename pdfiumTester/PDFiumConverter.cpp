// PDFicumConverter.cpp
#include "BuildConfig.h"
#include "PDFiumConverter.h"
#include <fpdfview.h> // FPDF_InitLibraryWithConfig, FPDF_DestroyLibrary, FPDF_InitLibraryWithConfig, FPDF_LoadDocument, FPDF_GetPageCount, FPDF_LoadPage
#include <fpdf_text.h> // FPDFText_LoadPage, FPDFText_CountChars, FPDFText_GetText
#include <fpdf_formfill.h> // FPDFDOC_InitFormFillEnvironment
#include <vector> // std::vector
#include <string> // std::string
#include <algorithm> // std::for_each
#include "pdf_utils.h" // AutoMemoryPtr
#include "fpdf_raii.h" // AutoFPDFDocumentPtr, AutoFPDFFormHandlePtr, AutoFPDFPagePtr, AutoFPDFTextPagePtr
#include "fpdf_converter.h" // fpdf::converter::WritePng

#ifdef _WIN32
#	include <gdiplus.h>
#   include <ppl.h> // concurrency::parallel_for_each, concurrency::critical_section
#else
#   include <string.h> // strdup
#   include <libgen.h> // dirname, basename
#   include <tbb/parallel_for_each.h> // tbb::parallel_for_each
#	include <tbb/critical_section.h> // tbb::critical_section
namespace concurrency {
	using tbb::parallel_for_each;
	using tbb::critical_section;
}
#endif

namespace PDF { namespace Converter {

#ifdef _WIN32
	static ULONG_PTR s_GdiplusToken = 0;
#endif

	PDFium::PDFium(const Flag& flag)
	: m_Flag(flag)
	{
	}

	PDFium::~PDFium()
	{
	}

	bool PDFium::Init()
	{
#ifdef _WIN32
		Gdiplus::GdiplusStartupInput gdiplusStartupInput = { 0, };
		Gdiplus::GdiplusStartup(&s_GdiplusToken, &gdiplusStartupInput, NULL);
#endif

		FPDF_LIBRARY_CONFIG config;

		config.version = 3;
		config.m_pUserFontPaths = nullptr;
		config.m_pIsolate = nullptr;
		config.m_v8EmbedderSlot = 0;
		config.m_pPlatform = nullptr;
		::FPDF_InitLibraryWithConfig(&config);

		return true;
	}

	void PDFium::Fini()
	{
#ifdef _WIN32
		Gdiplus::GdiplusShutdown(s_GdiplusToken);
#endif

		::FPDF_DestroyLibrary();
	}

	bool PDFium::ToImage(const wchar_t* sourceFile, const wchar_t* targetDir, int dpi /*= 96*/)
	{
		_ASSERTE(sourceFile && "sourceFile is not Null");
		_ASSERTE(targetDir && "sourceFile is not Null");
		if (!sourceFile || !targetDir) {
			return false;
		}

		FPDF_DOCUMENT fpdf_document = nullptr;
		AutoMemoryPtr memoryFile;

		if (m_Flag.test(FlagMemory)) {
			size_t buffer_len = 0;
			memoryFile = getFileContents(_U2A(sourceFile).c_str(), &buffer_len);
			fpdf_document = ::FPDF_LoadMemDocument(memoryFile.get(), static_cast<int>(buffer_len), nullptr);
		} else {
			fpdf_document = ::FPDF_LoadDocument(_U2A(sourceFile).c_str(), nullptr);
		}
		_ASSERTE(fpdf_document && "fpdf_document is not Null");
        if (!fpdf_document) {
            return false;
        }
		AutoFPDFDocumentPtr document(fpdf_document);

		FPDF_FORMHANDLE fpdf_form = FPDFDOC_InitFormFillEnvironment(document.get(), nullptr);
		AutoFPDFFormHandlePtr form(fpdf_form);

		{
			if (m_Flag.test(FlagPPL)) {
				struct FPDFPageItem
				{
					FPDFPageItem(int index, FPDF_PAGE page)
					: m_Index(index)
					, m_Page(page)
					{
					}

					AutoFPDFPagePtr m_Page;
					int m_Index;
				}; // struct FPDFPageItem

				// !!!!! Pdfium은 쓰레드 쎄이프 하지 않다.
				// https://groups.google.com/g/pdfium/c/HeZSsM_KEUk
				// 따라서 쓰레드 세이프 하진 않은 곳에 동기화 객체를 써주어야 한다.
				// == typedef std::shared_ptr<FPDFPageItem> FPDFPageItemPtr;
				using FPDFPageItemPtr = std::shared_ptr<FPDFPageItem>;
				std::vector<FPDFPageItemPtr> pageItemPtrVector;
				for (int i = 0; i < FPDF_GetPageCount(document.get()); i++) {
					FPDF_PAGE fpdf_page = ::FPDF_LoadPage(document.get(), i);
					_ASSERTE(fpdf_page && "fpdf_page is not Null");
					if (!fpdf_page) {
						continue;
					}
					// centos7의 기본 컴파일러 버전 4.8.5여서 std::vector::emplace_back이 없어
					// 복사생성자가 불리므로 std::unique_ptr을 std::shared_ptr로 한커플 씌움
					pageItemPtrVector.push_back(std::make_shared<FPDFPageItem>(i, fpdf_page));
				}

				concurrency::critical_section cs; // Pdfium이 쓰레드 세이프 하지 않아 최소한의 쓰레드 동기화를 한다.
				bool ret = true;
				concurrency::parallel_for_each(
					pageItemPtrVector.begin(),
					pageItemPtrVector.end(),
					[&](const FPDFPageItemPtr& pageItemPtr) {
						// PNG파일 추출
						{
							std::string resultPath = _U2A(targetDir) + std::to_string(pageItemPtr->m_Index) + ".png";
							ret = fpdf::converter::WritePng(cs, resultPath.c_str(), pageItemPtr->m_Page.get(), form.get(), static_cast<float>(dpi));
							_ASSERTE(ret && "fpdf::converter::WritePng() Failed");
						}
					}
				);
			} else {
				for (int pageIndex = 0; pageIndex < FPDF_GetPageCount(document.get()); pageIndex++) {
					FPDF_PAGE fpdf_page = ::FPDF_LoadPage(document.get(), pageIndex);
					_ASSERTE(fpdf_page && "fpdf_page is not Null");
					if (!fpdf_page) {
						continue;
					}
					AutoFPDFPagePtr page(fpdf_page);

					// PNG파일 추출
					{
						concurrency::critical_section cs;
						std::string resultPath = _U2A(targetDir) + std::to_string(pageIndex) + ".png";
						bool ret = fpdf::converter::WritePng(cs, resultPath.c_str(), page.get(), form.get(), static_cast<float>(dpi));
						_ASSERTE(ret && "fpdf::converter::WritePng() Failed");
						if (!ret) {
							continue;
						}
					}
				}
			}
		}

		return true;
	}

	bool PDFium::ToText(const wchar_t* sourceFile, const wchar_t* targetDir)
	{
		_ASSERTE(sourceFile && "sourceFile is not Null");
		_ASSERTE(targetDir && "sourceFile is not Null");
		if (!sourceFile || !targetDir) {
			return false;
		}

		const std::string samplePath = _U2A(sourceFile);
		const std::string rawFileName = removeExt(pathFindFilename(samplePath));
		const std::string resultPath = _U2A(targetDir) + rawFileName + ".txt";

		FPDF_DOCUMENT fpdf_document = nullptr;
		AutoMemoryPtr memoryFile;

		if (m_Flag.test(FlagMemory)) {
			size_t buffer_len = 0;
			memoryFile = getFileContents(samplePath.c_str(), &buffer_len);
			fpdf_document = ::FPDF_LoadMemDocument(memoryFile.get(), static_cast<int>(buffer_len), nullptr);
		} else {
			fpdf_document = ::FPDF_LoadDocument(samplePath.c_str(), nullptr);
		}
		_ASSERTE(fpdf_document && "fpdf_document is not Null");
        if (!fpdf_document) {
            return false;

        }
		AutoFPDFDocumentPtr document(fpdf_document);

		{
			// 텍스트파일 추출
			FILE* fp = fopen(resultPath.c_str(), "wb");
			_ASSERTE(fp && "fopen() Failed");
			if (!fp) {
				return false;        
			}
			unsigned short bom = 0xFEFF;
			fwrite(&bom, sizeof(bom), 1, fp);

			if (m_Flag.test(FlagPPL)) {
				struct FPDFPageTextItem
				{
					FPDFPageTextItem(int index, FPDF_PAGE page, FPDF_TEXTPAGE textPage)
					: m_Index(index)
					, m_TextPage(textPage)
					, m_Page(page)
					, m_Result()
					{
					}

					int m_Index;
					AutoFPDFPagePtr m_Page;
					AutoFPDFTextPagePtr m_TextPage;
					std::vector<unsigned short> m_Result;
				}; // struct FPDFPageTextItem

				// !!!!! Pdfium은 쓰레드 쎄이프 하지 않다.
				// https://groups.google.com/g/pdfium/c/HeZSsM_KEUk
				// 따라서 쓰레드 세이프 하진 않은 곳에 동기화 객체를 써주어야 한다.
				// == typedef std::shared_ptr<FPDFPageTextItem> FPDFPageTextItemPtr;
				using FPDFPageTextItemPtr = std::shared_ptr<FPDFPageTextItem>;
				std::vector<FPDFPageTextItemPtr> pageTextItemPtrVector;
				for (int i = 0; i < FPDF_GetPageCount(document.get()); i++) {
					FPDF_PAGE fpdf_page = ::FPDF_LoadPage(document.get(), i);
					_ASSERTE(fpdf_page && "fpdf_page is not Null");
					if (!fpdf_page) {
						continue;
					}

					FPDF_TEXTPAGE fpdf_textPage = ::FPDFText_LoadPage(fpdf_page);
					_ASSERTE(fpdf_textPage && "fpdf_textPage is not Null");
					if (!fpdf_textPage) {
						continue;
					}
					// centos7의 기본 컴파일러 버전 4.8.5여서 std::vector::emplace_back이 없어
					// 복사생성자가 불리므로 std::unique_ptr을 std::shared_ptr로 한커플 씌움
					pageTextItemPtrVector.push_back(std::make_shared<FPDFPageTextItem>(i, fpdf_page, fpdf_textPage));
				}

				// 병렬로 페이지의 텍스트 데이터를 복사한다.
				concurrency::parallel_for_each(
					pageTextItemPtrVector.begin(),
					pageTextItemPtrVector.end(),
					[&](const FPDFPageTextItemPtr& pageTextItemPtr) {
						// TXT 추출
						{
							FPDF_TEXTPAGE fpdf_textPage = pageTextItemPtr->m_TextPage.get();
							_ASSERTE(fpdf_textPage && "fpdf_textPage is not Null");
							if (!fpdf_textPage) {
								return;
							}
							std::vector<unsigned short>& result = pageTextItemPtr->m_Result;

							int countChars = ::FPDFText_CountChars(fpdf_textPage);
							unsigned short CRLF[2] = { '\r', '\n' };
							result.resize(countChars + (sizeof(CRLF) / sizeof(CRLF[0])));

							::FPDFText_GetText(fpdf_textPage, 0, static_cast<int>(countChars), &result[0]);

							memcpy(&result[countChars], CRLF, sizeof(CRLF));
						}
					}
				);

				// TXT 파일로 쓴다.
				std::for_each(
					pageTextItemPtrVector.begin(),
					pageTextItemPtrVector.end(),
					[&fp](const FPDFPageTextItemPtr& pageTextItemPtr) {
						const std::vector<unsigned short>& result = pageTextItemPtr->m_Result;
						fwrite(&result[0], result.size() * sizeof(unsigned short), 1, fp);
					}
				);
			} else {
				for (int pageIndex = 0; pageIndex < FPDF_GetPageCount(document.get()); pageIndex++) {
					FPDF_PAGE fpdf_page = ::FPDF_LoadPage(document.get(), pageIndex);
					_ASSERTE(fpdf_page && "fpdf_page is not Null");
					if (!fpdf_page) {
						continue;
					}
					AutoFPDFPagePtr page(fpdf_page);

					{
						FPDF_TEXTPAGE fpdf_textPage = ::FPDFText_LoadPage(page.get());
						_ASSERTE(fpdf_textPage && "fpdf_textPage is not Null");
						if (!fpdf_textPage) {
							continue;
						}
						AutoFPDFTextPagePtr textPage(fpdf_textPage);

						// TXT파일 추출
						{
							int countChars = ::FPDFText_CountChars(textPage.get());
							std::vector<unsigned short> result(countChars + 1, 0);
							::FPDFText_GetText(textPage.get(), 0, static_cast<int>(result.size()), &result[0]);
							fwrite(&result[0], countChars * sizeof(unsigned short), 1, fp);
							unsigned short CRLF[2] = { '\r', '\n' };
							fwrite(CRLF, sizeof(CRLF), 1, fp);
						}
					}
				}
			}
			
			fclose(fp);
		}

		return true;
	}

}} // PDF::Converter
