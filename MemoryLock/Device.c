/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, MemoryLockCreateDevice)
#endif

NTSTATUS UnlockEntry(LOCK_ENTRY* Entry);

NTSTATUS
MemoryLockCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    PAGED_CODE();
    WDF_OBJECT_ATTRIBUTES fileAttributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&fileAttributes);
    fileAttributes.SynchronizationScope = WdfSynchronizationScopeNone;
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&fileAttributes, FILE_CONTEXT);

    WDF_FILEOBJECT_CONFIG fileConfig;
    WDF_FILEOBJECT_CONFIG_INIT(
        &fileConfig,
        MemoryLockEvtDeviceFileCreate,
        MemoryLockEvtFileClose,
        WDF_NO_EVENT_CALLBACK // No cleanup callback function
    );

    WdfDeviceInitSetFileObjectConfig(DeviceInit, &fileConfig, &fileAttributes/*WDF_NO_OBJECT_ATTRIBUTES*/);

    // device init
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    WDFDEVICE device;
    NTSTATUS status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    DBGPRINT("[ML] WdfDeviceCreate = 0x%x\n", status);
    if (NT_SUCCESS(status)) {
        //
        // Get a pointer to the device context structure that we just associated
        // with the device object. We define this structure in the device.h
        // header file. DeviceGetContext is an inline function generated by
        // using the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in device.h.
        // This function will do the type checking and return the device context.
        // If you pass a wrong object handle it will return NULL and assert if
        // run under framework verifier mode.
        //
        DEVICE_CONTEXT* deviceContext = DeviceGetContext(device);

        //
        // Initialize private data.
        //
        deviceContext->LastRequestId = 0;

        //
        // Create a device interface so that applications can find and talk
        // to us.
        //
        status = WdfDeviceCreateDeviceInterface(
            device,
            &GUID_DEVINTERFACE_MemoryLock,
            NULL // ReferenceString
            );

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = MemoryLockQueueInitialize(device);
        }
    }

    return status;
}

void MemoryLockEvtDeviceFileCreate(
    WDFDEVICE Device,
    WDFREQUEST Request,
    WDFFILEOBJECT FileObject
)
{
    UNREFERENCED_PARAMETER(Device);

    FILE_CONTEXT* ctx = (FILE_CONTEXT*)WdfObjectGetTypedContext(FileObject, FILE_CONTEXT);
    InitializeListHead(&ctx->Requests);
    DBGPRINT("[ML] Device open: file %p, ctx %p, list %p\n", FileObject, ctx, &ctx->Requests);
    WdfRequestComplete(Request, STATUS_SUCCESS);
}

void MemoryLockEvtFileClose(
    WDFFILEOBJECT FileObject
)
{
    FILE_CONTEXT* ctx = (FILE_CONTEXT*)WdfObjectGetTypedContext(FileObject, FILE_CONTEXT);
    DBGPRINT("[ML] Device close: process %p, file %p, ctx %p\n", PsGetCurrentProcess(), FileObject, ctx);

    while (!IsListEmpty(&ctx->Requests))
    {
        LOCK_ENTRY* entry = CONTAINING_RECORD(ctx->Requests.Flink, LOCK_ENTRY, List);
        RemoveHeadList(&ctx->Requests);
        UnlockEntry(entry);
    }
    DbgPrint("[ML] DeviceClose <\n");
}