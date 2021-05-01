// PDFiumConverter.h
#pragma once
#include <bitset> // std::bitset

namespace PDF { namespace Converter {

	class PDFium
	{
	public:
		enum {
			FlagBegin = 0,

			FlagMemory = FlagBegin,
			FlagPPL,

			FlagEnd,
		}; // enum Flag 
		using Flag = std::bitset<FlagEnd>;

	public:
		PDFium(const Flag& flag);
		~PDFium();

	public:
		// 복사생성자/대입연산자/이동생성자/이동대입연산자는 
		// 컴파일러 기본을 따른다.
		PDFium(const PDFium&) = default;
		PDFium& operator=(const PDFium&) = default;
		PDFium(PDFium&&) = default;
		PDFium& operator=(PDFium&&) = default;

	public:
		static bool Init();
		static void Fini();

	public:
		bool ToImage(const wchar_t* sourceFile, const wchar_t* targetDir, int dpi = 96);
		bool ToText(const wchar_t* sourceFile, const wchar_t* targetDir);

	private:
		Flag m_Flag;
	}; // class PDFBox

}} // PDF::Converter
