#include "common.h"

#define QVMINI_TAG	'MMVQ'

PVOID AllocateMemory(
	ULONG uLength,
	PPFN_ARRAY pPfnArray
);

VOID FreeMemory(
	__in __drv_freesMem(Mem) PVOID pMemory
);

BOOLEAN GetUserBufferPfnArrayBool(
	PVOID pVirtualAddress,
	ULONG uLength,
	PPFN_ARRAY pPfnArray
);

PVOID AllocateSection(
	ULONG uLength,
    HANDLE *phSection,
	PVOID * pSectionObject,
	PVOID * ppMdl,
    PPFN_ARRAY pPfnArray,
    HANDLE *phDirtySection,
    PVOID *pDirtySectionObject,
    PVOID *pDirtySectionMemory
);

VOID FreeSection(
	HANDLE hSection,
    PVOID pSectionObject,
	PVOID pMdl,
    __in __drv_freesMem(Mem) PVOID BaseAddress,
    HANDLE hDirtySection,
    PVOID pDirtySectionObject,
    PVOID pDirtySectionMemory
);
