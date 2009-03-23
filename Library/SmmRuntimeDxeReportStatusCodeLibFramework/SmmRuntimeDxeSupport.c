/** @file
  Report Status Code Library for DXE Phase.

  Copyright (c) 2006 - 2007, Intel Corporation<BR>
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ReportStatusCodeLibInternal.h"

//
// Resources need by SMM runtime instance
// 
#include <Library/OemHookStatusCodeLib.h>
#include <Protocol/SmmBase.h>

EFI_EVENT             mVirtualAddressChangeEvent;

EFI_EVENT             mExitBootServicesEvent;

EFI_STATUS_CODE_DATA  *mStatusCodeData;

BOOLEAN               mInSmm;

EFI_SMM_BASE_PROTOCOL *mSmmBase;

EFI_RUNTIME_SERVICES  *mRT;

BOOLEAN               mHaveExitedBootServices = FALSE;

/**
  Locate he report status code service.

  @return     EFI_REPORT_STATUS_CODE    function point to
              ReportStatusCode.
**/
EFI_REPORT_STATUS_CODE
InternalGetReportStatusCode (
  VOID
  )
{
  EFI_STATUS_CODE_PROTOCOL  *StatusCodeProtocol;
  EFI_STATUS                Status;

  if (mInSmm) {
    return (EFI_REPORT_STATUS_CODE) OemHookStatusCodeReport;
  } else if (mRT != NULL && mRT->Hdr.Revision < 0x20000) {
    return ((FRAMEWORK_EFI_RUNTIME_SERVICES*)mRT)->ReportStatusCode;
  } else if (!mHaveExitedBootServices) {
  	//
  	// Check gBS just in case. ReportStatusCode is called before gBS is initialized.
  	//
    if (gBS != NULL) {
      Status = gBS->LocateProtocol (&gEfiStatusCodeRuntimeProtocolGuid, NULL, (VOID**)&StatusCodeProtocol);
      if (!EFI_ERROR (Status) && StatusCodeProtocol != NULL) {
        return StatusCodeProtocol->ReportStatusCode;
      }
    }
  }

  return NULL;
}


/**
  Fixup internal report status code protocol interface.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
ReportStatusCodeLibVirtualAddressChange (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  if (NULL != mReportStatusCode) {
    mRT->ConvertPointer (0, (VOID **) &mReportStatusCode);
  }
  mRT->ConvertPointer (0, (VOID **) &mStatusCodeData);
  mRT->ConvertPointer (0, (VOID **) &mRT);
}

/**
  Update the In Runtime Indicator.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
ReportStatusCodeLibExitBootServices (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  mHaveExitedBootServices = TRUE;
}

/**
  Intialize Report Status Code Lib.

  @param[in]  ImageHandle   The firmware allocated handle for the EFI image.
  @param[in]  SystemTable   A pointer to the EFI System Table.

  @return     EFI_STATUS    always returns EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
ReportStatusCodeLibConstruct (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS            Status;

  //
  // SMM driver depends on the SMM BASE protocol.
  // the SMM driver must be success to locate protocol.
  // 
  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &mSmmBase);
  if (!EFI_ERROR (Status)) {
    mSmmBase->InSmm (mSmmBase, &mInSmm);
    if (mInSmm) {
      Status = mSmmBase->SmmAllocatePool (
                           mSmmBase,
                           EfiRuntimeServicesData, 
                           sizeof (EFI_STATUS_CODE_DATA) + EFI_STATUS_CODE_DATA_MAX_SIZE, 
                           (VOID **) &mStatusCodeData
                           );
      ASSERT_EFI_ERROR (Status);
      OemHookStatusCodeInitialize ();
      return EFI_SUCCESS;
    }
  }

  //
  // Library should not use the gRT directly, since it
  // may be converted by other library instance.
  // 
  mRT     = gRT;
  mInSmm  = FALSE;

  gBS->AllocatePool (EfiRuntimeServicesData, sizeof (EFI_STATUS_CODE_DATA) + EFI_STATUS_CODE_DATA_MAX_SIZE, (VOID **)&mStatusCodeData);
  ASSERT (NULL != mStatusCodeData);
  //
  // Cache the report status code service
  // 
  mReportStatusCode = InternalGetReportStatusCode ();

  //
  // Register the call back of virtual address change
  // 
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  ReportStatusCodeLibVirtualAddressChange,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);


  //
  // Register the call back of virtual address change
  // 
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  ReportStatusCodeLibExitBootServices,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Desctructor of library will close events.
  
  @param ImageHandle callder module's image handle
  @param SystemTable pointer to EFI system table.
  @return the status of close event.
**/
EFI_STATUS
EFIAPI
ReportStatusCodeLibDestruct (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  if (!mInSmm) {
    //
    // Close SetVirtualAddressMap () notify function
    //
    ASSERT (gBS != NULL);
    Status = gBS->CloseEvent (mVirtualAddressChangeEvent);
    ASSERT_EFI_ERROR (Status);
    Status = gBS->CloseEvent (mExitBootServicesEvent);
    ASSERT_EFI_ERROR (Status);

    gBS->FreePool (mStatusCodeData);
  } else {
    mSmmBase->SmmFreePool (mSmmBase, mStatusCodeData);
  }

  return EFI_SUCCESS;
}

/**
  Reports a status code with full parameters.

  The function reports a status code.  If ExtendedData is NULL and ExtendedDataSize
  is 0, then an extended data buffer is not reported.  If ExtendedData is not
  NULL and ExtendedDataSize is not 0, then an extended data buffer is allocated.
  ExtendedData is assumed not have the standard status code header, so this function
  is responsible for allocating a buffer large enough for the standard header and
  the extended data passed into this function.  The standard header is filled in
  with a GUID specified by ExtendedDataGuid.  If ExtendedDataGuid is NULL, then a
  GUID of gEfiStatusCodeSpecificDatauid is used.  The status code is reported with
  an instance specified by Instance and a caller ID specified by CallerId.  If
  CallerId is NULL, then a caller ID of gEfiCallerIdGuid is used.

  ReportStatusCodeEx()must actively prevent recursion.  If ReportStatusCodeEx()
  is called while processing another any other Report Status Code Library function,
  then ReportStatusCodeEx() must return EFI_DEVICE_ERROR immediately.

  If ExtendedData is NULL and ExtendedDataSize is not zero, then ASSERT().
  If ExtendedData is not NULL and ExtendedDataSize is zero, then ASSERT().

  @param  Type              Status code type.
  @param  Value             Status code value.
  @param  Instance          Status code instance number.
  @param  CallerId          Pointer to a GUID that identifies the caller of this
                            function.  If this parameter is NULL, then a caller
                            ID of gEfiCallerIdGuid is used.
  @param  ExtendedDataGuid  Pointer to the GUID for the extended data buffer.
                            If this parameter is NULL, then a the status code
                            standard header is filled in with
                            gEfiStatusCodeSpecificDataGuid.
  @param  ExtendedData      Pointer to the extended data buffer.  This is an
                            optional parameter that may be NULL.
  @param  ExtendedDataSize  The size, in bytes, of the extended data buffer.

  @retval  EFI_SUCCESS           The status code was reported.
  @retval  EFI_OUT_OF_RESOURCES  There were not enough resources to allocate
                                 the extended data section if it was specified.
  @retval  EFI_UNSUPPORTED       Report status code is not supported

**/
EFI_STATUS
EFIAPI
InternalReportStatusCodeEx (
  IN EFI_STATUS_CODE_TYPE   Type,
  IN EFI_STATUS_CODE_VALUE  Value,
  IN UINT32                 Instance,
  IN CONST EFI_GUID         *CallerId          OPTIONAL,
  IN CONST EFI_GUID         *ExtendedDataGuid  OPTIONAL,
  IN CONST VOID             *ExtendedData      OPTIONAL,
  IN UINTN                  ExtendedDataSize
  )
{
  ASSERT (!((ExtendedData == NULL) && (ExtendedDataSize != 0)));
  ASSERT (!((ExtendedData != NULL) && (ExtendedDataSize == 0)));

  if (ExtendedDataSize > EFI_STATUS_CODE_DATA_MAX_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Fill in the extended data header
  //
  mStatusCodeData->HeaderSize = sizeof (EFI_STATUS_CODE_DATA);
  mStatusCodeData->Size = (UINT16)ExtendedDataSize;
  if (ExtendedDataGuid == NULL) {
    ExtendedDataGuid = &gEfiStatusCodeSpecificDataGuid;
  }
  CopyGuid (&mStatusCodeData->Type, ExtendedDataGuid);

  //
  // Fill in the extended data buffer
  //
  if (ExtendedData != NULL) {
    CopyMem (mStatusCodeData + 1, ExtendedData, ExtendedDataSize);
  }

  //
  // Report the status code
  //
  if (CallerId == NULL) {
    CallerId = &gEfiCallerIdGuid;
  }
  return  InternalReportStatusCode (Type, Value, Instance, CallerId, mStatusCodeData);
}

