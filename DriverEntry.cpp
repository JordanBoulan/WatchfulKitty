

#include "WatchfulKitty.h"


void* __cdecl operator new(unsigned __int64 size) {
	PHYSICAL_ADDRESS highest;

	highest.QuadPart = 0xFFFFFFFFFFFFFFFF;
	return MmAllocateContiguousMemory(size, highest);
}

typedef struct _DPC_CONTEXT {
	ULONG_PTR Cr3;
} DPC_CONTEXT, *PDPC_CONTEXT;

EXTERN_C
NTKERNELAPI
_IRQL_requires_max_(APC_LEVEL)
_IRQL_requires_min_(PASSIVE_LEVEL)
_IRQL_requires_same_
VOID
KeGenericCallDpc(
	_In_ PKDEFERRED_ROUTINE Routine,
	_In_opt_ PVOID Context
);

EXTERN_C
NTKERNELAPI
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
KeSignalCallDpcDone(
	_In_ PVOID SystemArgument1
);

EXTERN_C
NTKERNELAPI
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
LOGICAL
KeSignalCallDpcSynchronize(
	_In_ PVOID SystemArgument2
);




VOID VTLoadDpc(
	_In_ struct _KDPC *Dpc,
	_In_opt_ PVOID params,
	_In_opt_ PVOID numcpu,
	_In_opt_ PVOID barrier
)
{
	UNREFERENCED_PARAMETER(Dpc);
	KeSignalCallDpcSynchronize(barrier); //we must do this before we access any of our data or a race condition may occur
	PDPC_CONTEXT ctx = (PDPC_CONTEXT)params;
	//The orginal parameters to our function
	//all other parmeters handled by the kernel

	DbgPrint("Current Processor : %d\n", KeGetCurrentProcessorIndex());

	SimpleVT* vt = new SimpleVT();
	vt->SetRootmodeCR3(ctx->Cr3);

	if (vt->Install()) {
		DbgPrint("Success on a processor!\n");

		KeSignalCallDpcSynchronize(barrier);
		KeSignalCallDpcDone(numcpu);
	}
	else {
		DbgPrint("Failed on a processor!\n");
	}

	

	return;
}

VOID VTLoad(void)
{
	DPC_CONTEXT ctx;

	ctx.Cr3 = __readcr3();

	KeGenericCallDpc(VTLoadDpc, &ctx);
	return;
}





//expand be legit when we expand our stack so "driver-verifier" doesn't get mad, otherwise we have to low-level hack (DKOM) the windows pfn (page-frame-number) database (The windows virt mem system)




#ifdef __cplusplus
#if __cplusplus
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING ustrRegPath)
#else
NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING ustrRegPath)
#endif
#endif 
{
	UNREFERENCED_PARAMETER(pDrvObj);
	UNREFERENCED_PARAMETER(ustrRegPath);

	DbgPrint("Driver Entry\n");

	//SimpleVT* vt = new SimpleVT();
	//vt->SetRootmodeCR3(__readcr3());
	//if (vt->Install()) {
	//	DbgPrint("install Success!\n");
	//}
	///else {
	//	DbgPrint("Failed!\n");
	//}

	VTLoad();

	



	DbgPrint("Exit Driver Entry\n");

	return STATUS_SUCCESS;
}
