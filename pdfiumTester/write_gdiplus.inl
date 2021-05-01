// write_gdiplus.inl
#pragma once

#include <fpdfview.h> // FPDF_GetPageWidthF, FPDF_GetPageHeightF, FPDFBitmap_Create, 
                      // FPDFBitmap_Create, FPDFBitmap_FillRect, FPDF_RenderPageBitmap, 
                      // FPDFBitmap_GetStride, FPDFBitmap_GetBuffer
#include <fpdf_edit.h> // FPDFPage_HasTransparency
#include <fpdf_formfill.h> // FPDF_FFLDraw
#include <gdiplus.h>
#include <atlbase.h> // ATL::CComPtr
#include <atlconv.h> // ATL::CA2W
#include <mutex> // std::mutex
#include "fpdf_raii.h" // AutoFPDFBitmapPtr
// #include <span> // std::span (C++20)
#include "span.h"
#include <stdint.h> // uint8_t
namespace std {
    template<typename T>
    using span = typename tcb::span<T>;
}

namespace gdiplus {

    auto _getEncoderClsid = [](const wchar_t* format, CLSID* pClsid) -> int {
        using namespace ::Gdiplus;

        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        ImageCodecInfo* pImageCodecInfo = NULL;

        GetImageEncodersSize(&num, &size);
        if (size == 0)
            return -1;  // Failure

        pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        if (pImageCodecInfo == NULL)
            return -1;  // Failure

        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j) {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;  // Success
            }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
    };

    inline std::vector<uint8_t> EncodePng(
        const std::span<const uint8_t>& input,
        int width,
        int height,
        int stride,
        int format
    ) {
        CLSID pngClsid = { 0, };
        int result = _getEncoderClsid(L"image/png", &pngClsid);
        _ASSERTE(result != -1 && "_getEncoderClsid() Failed");
        if (result == -1) {
            return std::vector<uint8_t>();
        }

        Gdiplus::Bitmap gBitmap(width, height, stride, format, (BYTE*)input.data());

        // write to IStream
        ATL::CComPtr<IStream> spStreamPtr;
        HRESULT hr = ::CreateStreamOnHGlobal(NULL, TRUE, &spStreamPtr);
        _ASSERTE(SUCCEEDED(hr) && "CreateStreamOnHGlobal() Failed");
        _ASSERTE(spStreamPtr && "spStreamPtr is not Null");
        if (FAILED(hr) || !spStreamPtr) {
            return std::vector<uint8_t>();
        }

        Gdiplus::Status status = gBitmap.Save(spStreamPtr, &pngClsid);
        _ASSERTE(status == Gdiplus::Status::Ok && "gBitmap.Save() Failed");
        if (status != Gdiplus::Status::Ok) {
            return std::vector<uint8_t>();
        }

        HGLOBAL hGlobal = nullptr;
         hr = ::GetHGlobalFromStream(spStreamPtr, &hGlobal);
        _ASSERTE(SUCCEEDED(hr) && "GetHGlobalFromStream() Failed");
        _ASSERTE(hGlobal && "hGlobal is not Null");
        if (FAILED(hr) || nullptr == hGlobal) {
            return std::vector<uint8_t>();
        }

        // copy IStream to buffer
        std::vector<uint8_t> output;
        SIZE_T bufsize = ::GlobalSize(hGlobal);
        output.resize(bufsize);

        // lock & unlock memory
        LPVOID pImage = ::GlobalLock(hGlobal);
        ::memcpy(&output[0], pImage, bufsize);
        ::GlobalUnlock(hGlobal);

        return output;
    }

    inline bool WritePng(std::mutex& mtx, const char* pathName, FPDF_PAGE page, FPDF_FORMHANDLE form = nullptr, float dpi = 96.F)
    {
        _ASSERTE(pathName && "pathName is not Null");
        _ASSERTE(page && "page is not Null");
        if (!pathName || !page) {
            return false;
        }

        float pageWidth = FPDF_GetPageWidthF(page);
        float pageHeight = FPDF_GetPageHeightF(page);
        int width = static_cast<int>(pageWidth / 72.F * dpi);
        int height = static_cast<int>(pageHeight / 72.F * dpi);

        int alpha = FPDFPage_HasTransparency(page) ? 1 : 0;
        FPDF_BITMAP fpdf_bitmap = FPDFBitmap_Create(width, height, alpha);
        _ASSERTE(fpdf_bitmap && "fpdf_bitmap is not Null");
        if (!fpdf_bitmap) {
            return false;
        }
        AutoFPDFBitmapPtr bitmap(fpdf_bitmap);
        
        FPDF_DWORD fill_color = alpha ? 0x00000000 : 0xFFFFFFFF;
        FPDFBitmap_FillRect(bitmap.get(), 0, 0, width, height, fill_color);
        int flags = 0;

        mtx.lock();
        {
            FPDF_RenderPageBitmap(bitmap.get(), page, 0, 0, width, height, 0, flags);
            FPDF_FFLDraw(form, bitmap.get(), page, 0, 0, width, height, 0, flags);
        }
        mtx.unlock();

        int stride = FPDFBitmap_GetStride(bitmap.get());
        void* buffer = FPDFBitmap_GetBuffer(bitmap.get());

        {
            

            Gdiplus::Bitmap gBitmap(width, height, stride, PixelFormat32bppARGB, (BYTE*)buffer);
            CLSID pngClsid = {0, };
            int result = _getEncoderClsid(L"image/png", &pngClsid);
            _ASSERTE(result != -1 && "_getEncoderClsid() Failed");
            if (result == -1) {
                return false;
            }
            Gdiplus::Status status = gBitmap.SetResolution(dpi, dpi);
            _ASSERTE(status == Gdiplus::Status::Ok && "gBitmap.SetResolution() Failed");
            if (status != Gdiplus::Status::Ok) {
                return false;
            }

            status = gBitmap.Save(ATL::CA2W(pathName), &pngClsid, NULL);
            _ASSERTE(status == Gdiplus::Status::Ok && "gBitmap.Save() Failed");
            if (status != Gdiplus::Status::Ok) {
                return false;
            }
        }

        return true;
    }

} // gdiplus
