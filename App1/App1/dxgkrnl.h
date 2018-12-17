#pragma once
#include <windows.h>
typedef UINT D3DKMT_HANDLE;
typedef UINT  D3DDDI_VIDEO_PRESENT_SOURCE_ID;
typedef ULONGLONG D3DGPU_VIRTUAL_ADDRESS;

typedef struct _D3DKMT_STANDARDALLOCATION_EXISTINGHEAP
{
	SIZE_T Size;        // in: Size in bytes of existing heap
} D3DKMT_STANDARDALLOCATION_EXISTINGHEAP;

typedef struct _D3DKMT_CREATEALLOCATIONFLAGS
{
	UINT    CreateResource : 1;    // 0x00000001
	UINT    CreateShared : 1;    // 0x00000002
	UINT    NonSecure : 1;    // 0x00000004
	UINT    CreateProtected : 1;    // 0x00000008 Cannot be used when allocation is created from the user mode.
	UINT    RestrictSharedAccess : 1;    // 0x00000010
	UINT    ExistingSysMem : 1;    // 0x00000020 Cannot be used when allocation is created from the user mode.
#if (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN8)
	UINT    NtSecuritySharing : 1;    // 0x00000040
	UINT    ReadOnly : 1;    // 0x00000080
	UINT    CreateWriteCombined : 1;    // 0x00000100 Cannot be used when allocation is created from the user mode.
	UINT    CreateCached : 1;    // 0x00000200 Cannot be used when allocation is created from the user mode.
	UINT    SwapChainBackBuffer : 1;    // 0x00000400 Specifies whether an allocation corresponds to a swap chain back buffer.
#if (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM1_3)
	UINT    CrossAdapter : 1;    // 0x00000800
	UINT    OpenCrossAdapter : 1;    // 0x00001000 Cannot be used when allocation is created from the user mode.
	UINT    PartialSharedCreation : 1;    // 0x00002000 
	UINT    Zeroed : 1;    // 0x00004000  // in: specify request for zeroed pages, out: set when allocation fulfilled by zero pages
	UINT    WriteWatch : 1;    // 0x00008000  // in: request Mm to track writes to pages of this allocation
#if (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM2_3)
	UINT    StandardAllocation : 1;    // 0x00010000  // in: use pStandardAllocation instead of pPrivateDriverData
	UINT    ExistingSection : 1;    // 0x00020000  // in: Use Section Handle instead of SysMem in D3DDI_ALLOCATIONINFO2
	UINT    Reserved : 14;    // 0xFFFD0000
#else
	UINT    Reserved : 16;    // 0xFFFF0000
#endif //(DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM2_3)
#else
	UINT    Zeroed : 1;    // 0x00000800  // in: specify request for zeroed pages, out: set when allocation fulfilled by zero pages
	UINT    Reserved : 20;    // 0xFFFFF000
#endif // (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM1_3)
#else
	UINT    Zeroed : 1;    // 0x00000040  // in: specify request for zeroed pages, out: set when allocation fulfilled by zero pages
	UINT    Reserved : 25;    // 0xFFFFFF80
#endif
} D3DKMT_CREATEALLOCATIONFLAGS;

typedef struct _D3DKMT_CREATESTANDARDALLOCATIONFLAGS
{
	union
	{
		struct
		{
			UINT Reserved : 32; // 0xFFFFFFFF
		};
		UINT Value;
	};
} D3DKMT_CREATESTANDARDALLOCATIONFLAGS;


typedef enum _D3DKMT_STANDARDALLOCATIONTYPE
{
	D3DKMT_STANDARDALLOCATIONTYPE_EXISTINGHEAP = 1,
} D3DKMT_STANDARDALLOCATIONTYPE;

typedef struct _D3DKMT_CREATESTANDARDALLOCATION
{
	// 
	// update onecoreuap/windows/core/ntuser/inc/whwin32.tpl when adding new memeber
	// to this struct
	//
	D3DKMT_STANDARDALLOCATIONTYPE Type;
	union
	{
		D3DKMT_STANDARDALLOCATION_EXISTINGHEAP ExistingHeapData;
	};
	D3DKMT_CREATESTANDARDALLOCATIONFLAGS Flags;
} D3DKMT_CREATESTANDARDALLOCATION;

typedef struct _D3DDDI_ALLOCATIONINFO
{
	D3DKMT_HANDLE                   hAllocation;           // out: Private driver data for allocation
	CONST VOID*                     pSystemMem;            // in: Pointer to pre-allocated sysmem
	VOID*                           pPrivateDriverData;    // in(out optional): Private data for each allocation
	UINT                            PrivateDriverDataSize; // in: Size of the private data
	D3DDDI_VIDEO_PRESENT_SOURCE_ID  VidPnSourceId;         // in: VidPN source ID if this is a primary
	union
	{
		struct
		{
			UINT    Primary : 1;    // 0x00000001
#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN8) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WIN8))
			UINT    Stereo : 1;    // 0x00000002
			UINT    Reserved : 30;    // 0xFFFFFFFC
#else
			UINT    Reserved : 31;    // 0xFFFFFFFE
#endif // (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN8)
		};
		UINT        Value;
	} Flags;
} D3DDDI_ALLOCATIONINFO;

typedef struct _D3DDDI_ALLOCATIONINFO2
{
	D3DKMT_HANDLE                   hAllocation;           // out: Private driver data for allocation
	union
	{
		HANDLE                      hSection;              // in: Handle to valid section object
		CONST VOID*                 pSystemMem;            // in: Pointer to pre-allocated sysmem
	};
	VOID*                           pPrivateDriverData;    // in(out optional): Private data for each allocation
	UINT                            PrivateDriverDataSize; // in: Size of the private data
	D3DDDI_VIDEO_PRESENT_SOURCE_ID  VidPnSourceId;         // in: VidPN source ID if this is a primary
	union
	{
		struct
		{
			UINT    Primary : 1;    // 0x00000001
#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN8) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WIN8))
			UINT    Stereo : 1;    // 0x00000002
#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM2_2) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WDDM2_2))
			UINT    OverridePriority : 1;    // 0x00000004
			UINT    Reserved : 29;   // 0xFFFFFFF8
#else
			UINT    Reserved : 30;    // 0xFFFFFFFC
#endif
#else
			UINT    Reserved : 31;    // 0xFFFFFFFE
#endif // (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN8)
		};
		UINT        Value;
	} Flags;
	D3DGPU_VIRTUAL_ADDRESS          GpuVirtualAddress;    // out: GPU Virtual address of the allocation created.
#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM2_2) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WDDM2_2))
	union
	{
		UINT                        Priority;             // in: priority of allocation
		ULONG_PTR                   Unused;
	};
	ULONG_PTR                       Reserved[5];          // Reserved
#else
	ULONG_PTR                       Reserved[6];          // Reserved
#endif
} D3DDDI_ALLOCATIONINFO2;

typedef struct _D3DKMT_CREATEALLOCATION
{
	D3DKMT_HANDLE                   hDevice;
	D3DKMT_HANDLE                   hResource;      //in/out:valid only within device
	D3DKMT_HANDLE                   hGlobalShare;   //out:Shared handle if CreateShared and not NtSecuritySharing
	_Field_size_bytes_(PrivateRuntimeDataSize)
		CONST VOID*                     pPrivateRuntimeData;
	UINT                            PrivateRuntimeDataSize;
	union
	{
		// 
		// update onecoreuap/windows/core/ntuser/inc/whwin32.tpl when adding new memeber
		// to this union
		//
		D3DKMT_CREATESTANDARDALLOCATION* pStandardAllocation;
		_Field_size_bytes_(PrivateDriverDataSize)
			CONST VOID*                       pPrivateDriverData;
	};
	UINT                            PrivateDriverDataSize;
	UINT                            NumAllocations;
	union
	{
		_Field_size_(NumAllocations)       D3DDDI_ALLOCATIONINFO*   pAllocationInfo;
#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN7) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WIN7))
		_Field_size_(NumAllocations)       D3DDDI_ALLOCATIONINFO2*  pAllocationInfo2; // _ADVSCH_
#endif
	};
	D3DKMT_CREATEALLOCATIONFLAGS    Flags;
	HANDLE                          hPrivateRuntimeResourceHandle; // opaque handle used for event tracing
} D3DKMT_CREATEALLOCATION;

typedef NTSTATUS(*D3DKMTCreateAllocation_t)(
	D3DKMT_CREATEALLOCATION *Arg1
	);
