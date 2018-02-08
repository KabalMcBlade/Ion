// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ION_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ION_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ION_EXPORTS
#define ION_API __declspec(dllexport)
#else
#define ION_API __declspec(dllimport)
#endif

// This class is exported from the Ion.dll
class ION_API CIon {
public:
	CIon(void);
	// TODO: add your methods here.
};

extern ION_API int nIon;

ION_API int fnIon(void);
