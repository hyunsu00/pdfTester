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
		static bool Init();
		static void Fini();

	public:
		bool ToImage(const wchar_t* sourceFile, const wchar_t* targetDir, int dpi = 96);
		bool ToText(const wchar_t* sourceFile, const wchar_t* targetDir);

	private:
		Flag m_Flag;
	}; // class PDFBox

}} // PDF::Converter
