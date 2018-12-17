#include "pch.h"

#include "dxgkrnl.h"
#include "init_list.h"
D3DKMTCreateAllocation_t D3DKMTCreateAllocation;
extern unsigned wp_version;
//int d3dinit(unsigned gdi32_base)
//{
//	if (wp_version == WP_15063_1446)
//		D3DKMTCreateAllocation = (D3DKMTCreateAllocation_t)(gdi32_base + 0x000034A1);
//}

//int d3dca()
//{
//	D3DKMT_CREATEALLOCATION allocation = { 0 };
//	allocation.hDevice = ;
//	allocation.hResource = NULL;
//	allocation.NumAllocations = 1;
//	D3DDDI_ALLOCATIONINFO2 allocationInfo = { 0 };
//	allocationInfo.VidPnSourceId = 0;
//	allocationInfo.Flags.OverridePriority = 1;
//
//	allocation.Flags.CreateResource = 1;
//	allocation.Flags.CreateShared = 1;
//	//allocation.Flags.CrossAdapter = 1;
//	D3DKMTCreateAllocation(&allocation);
//	return 0;
//}