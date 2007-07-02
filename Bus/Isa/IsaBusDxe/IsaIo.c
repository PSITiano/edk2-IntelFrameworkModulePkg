/*++
 
Copyright (c) 2006 - 2007, Intel Corporation. All rights reserved. <BR> 
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IsaIo.c
  
Abstract:

  The implementation for EFI_ISA_IO_PROTOCOL. 

--*/

//
// Include common header file for this module.
//
#include "InternalIsaIo.h"

#include <IndustryStandard/Pcat.h>

//
// Driver Support Global Variables
//
EFI_ISA_IO_PROTOCOL IsaIoInterface = {
  {    
    IsaIoMemRead,
    IsaIoMemWrite
  },
  {   
    IsaIoIoRead,
    IsaIoIoWrite
  },
  IsaIoCopyMem,
  IsaIoMap,
  IsaIoUnmap,
  IsaIoAllocateBuffer,
  IsaIoFreeBuffer,
  IsaIoFlush,
  NULL,
  0,
  NULL
};

static EFI_ISA_DMA_REGISTERS  DmaRegisters[8] = {
  {
    0x00,
    0x87,
    0x01
  },
  {
    0x02,
    0x83,
    0x03
  },
  {
    0x04,
    0x81,
    0x05
  },
  {
    0x06,
    0x82,
    0x07
  },
  {
    0x00,
    0x00,
    0x00
  },  // Channel 4 is invalid
  {
    0xC4,
    0x8B,
    0xC6
  },
  {
    0xC8,
    0x89,
    0xCA
  },
  {
    0xCC,
    0x8A,
    0xCE
  },
};

EFI_STATUS
ReportErrorStatusCode (
  EFI_STATUS_CODE_VALUE Code
  )
/*++

Routine Description:

  report a error Status code of PCI bus driver controller

Arguments:

  Code         - The error status code.
  
Returns:

  EFI_SUCCESS  - Success to report status code.
  

--*/
{
  return REPORT_STATUS_CODE (
                EFI_ERROR_CODE | EFI_ERROR_MINOR,
                Code
                );
}

//
// Driver Support Functions
//

EFI_STATUS
InitializeIsaIoInstance (
  IN ISA_IO_DEVICE               *IsaIoDevice,
  IN EFI_ISA_ACPI_RESOURCE_LIST  *IsaDeviceResourceList
  )
/*++

Routine Description:

  Initializes an ISA I/O Instance

Arguments:

  IsaIoDevice            - The iso device to be initialized.
  IsaDeviceResourceList  - The resource list.
  
Returns:

  EFI_SUCCESS            - Initial success.
  
--*/
{
  //
  // Initializes an ISA I/O Instance
  //
  CopyMem (
    &IsaIoDevice->IsaIo,
    &IsaIoInterface,
    sizeof (EFI_ISA_IO_PROTOCOL)
    );

  IsaIoDevice->IsaIo.ResourceList = IsaDeviceResourceList;
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
IsaIoIoRead (
  IN     EFI_ISA_IO_PROTOCOL                        *This,
  IN     EFI_ISA_IO_PROTOCOL_WIDTH                  Width,
  IN     UINT32                                     Offset,
  IN     UINTN                                      Count,
  IN OUT VOID                                       *Buffer
  )
/*++

Routine Description:

  Performs an ISA I/O Read Cycle

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Width                 - Signifies the width of the I/O operation.
  Offset                - The offset in ISA I/O space to start the I/O operation.  
  Count                 - The number of I/O operations to perform. 
  Buffer                - The destination buffer to store the results

Returns:

  EFI_SUCCESS           - The data was read from the device sucessfully.
  EFI_UNSUPPORTED       - The Offset is not valid for this device.
  EFI_INVALID_PARAMETER - Width or Count, or both, were invalid.
  EFI_OUT_OF_RESOURCES  - The request could not be completed due to a lack of resources.

--*/
{
  EFI_STATUS    Status;
  ISA_IO_DEVICE *IsaIoDevice;

  IsaIoDevice = ISA_IO_DEVICE_FROM_ISA_IO_THIS (This);

  //
  // Verify Isa IO Access
  //
  Status = IsaIoVerifyAccess (
             IsaIoDevice,
             IsaAccessTypeIo,
             Width,
             Count,
             &Offset
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Call PciIo->Io.Read
  //
  Status = IsaIoDevice->PciIo->Io.Read (
                                    IsaIoDevice->PciIo,
                                    (EFI_PCI_IO_PROTOCOL_WIDTH) Width,
                                    EFI_PCI_IO_PASS_THROUGH_BAR,
                                    Offset,
                                    Count,
                                    Buffer
                                    );

  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
  }

  return Status;
}

EFI_STATUS
EFIAPI
IsaIoIoWrite (
  IN     EFI_ISA_IO_PROTOCOL                        *This,
  IN     EFI_ISA_IO_PROTOCOL_WIDTH                  Width,
  IN     UINT32                                     Offset,
  IN     UINTN                                      Count,
  IN OUT VOID                                       *Buffer
  )
/*++

Routine Description:

  Performs an ISA I/O Write Cycle

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Width                 - Signifies the width of the I/O operation.
  Offset                - The offset in ISA I/O space to start the I/O operation.  
  Count                 - The number of I/O operations to perform. 
  Buffer                - The source buffer to write data from

Returns:

  EFI_SUCCESS           - The data was writen to the device sucessfully.
  EFI_UNSUPPORTED       - The Offset is not valid for this device.
  EFI_INVALID_PARAMETER - Width or Count, or both, were invalid.
  EFI_OUT_OF_RESOURCES  - The request could not be completed due to a lack of resources.

--*/
{
  EFI_STATUS    Status;
  ISA_IO_DEVICE *IsaIoDevice;

  IsaIoDevice = ISA_IO_DEVICE_FROM_ISA_IO_THIS (This);

  //
  // Verify Isa IO Access
  //
  Status = IsaIoVerifyAccess (
             IsaIoDevice,
             IsaAccessTypeIo,
             Width,
             Count,
             &Offset
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Call PciIo->Io.Write
  //
  Status = IsaIoDevice->PciIo->Io.Write (
                                    IsaIoDevice->PciIo,
                                    (EFI_PCI_IO_PROTOCOL_WIDTH) Width,
                                    EFI_PCI_IO_PASS_THROUGH_BAR,
                                    Offset,
                                    Count,
                                    Buffer
                                    );

  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
  }

  return Status;
}

EFI_STATUS
WritePort (
  IN EFI_ISA_IO_PROTOCOL                  *This,
  IN UINT32                               Offset,
  IN UINT8                                Value
  )
/*++

Routine Description:

  Writes an 8 bit I/O Port

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Offset                - The offset in ISA IO space to start the IO operation.  
  Value                 - The data to write port.
  
Returns:

  EFI_SUCCESS           - Success.
  EFI_INVALID_PARAMETER - Parameter is invalid.
  EFI_UNSUPPORTED       - The address range specified by Offset is not valid.
  EFI_OUT_OF_RESOURCES  - The request could not be completed due to a lack of resources.
  
--*/
{
  EFI_STATUS    Status;
  ISA_IO_DEVICE *IsaIoDevice;

  IsaIoDevice = ISA_IO_DEVICE_FROM_ISA_IO_THIS (This);

  //
  // Call PciIo->Io.Write
  //
  Status = IsaIoDevice->PciIo->Io.Write (
                                    IsaIoDevice->PciIo,
                                    EfiPciIoWidthUint8,
                                    EFI_PCI_IO_PASS_THROUGH_BAR,
                                    Offset,
                                    1,
                                    &Value
                                    );
  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
    return Status;
  }

  gBS->Stall (50);

  return EFI_SUCCESS;
}

EFI_STATUS
WriteDmaPort (
  IN EFI_ISA_IO_PROTOCOL                  *This,
  IN UINT32                               AddrOffset,
  IN UINT32                               PageOffset,
  IN UINT32                               CountOffset,
  IN UINT32                               BaseAddress,
  IN UINT16                               Count
  )
/*++

Routine Description:

  Writes I/O operation base address and count number to a 8 bit I/O Port.

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  AddrOffset            - The address' offset.
  PageOffset            - The page's offest.
  CountOffset           - The count's offset.
  BaseAddress           - The base address.
  Count                 - The number of I/O operations to perform. 
  
Returns:

  EFI_SUCCESS           - Success.
  EFI_INVALID_PARAMETER - Parameter is invalid.
  EFI_UNSUPPORTED       - The address range specified by these Offsets and Count is not valid.
  EFI_OUT_OF_RESOURCES  - The request could not be completed due to a lack of resources.

--*/
{
  EFI_STATUS  Status;

  Status = WritePort (This, AddrOffset, (UINT8) (BaseAddress & 0xff));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (This, AddrOffset, (UINT8) ((BaseAddress >> 8) & 0xff));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (This, PageOffset, (UINT8) ((BaseAddress >> 16) & 0xff));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (This, CountOffset, (UINT8) (Count & 0xff));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (This, CountOffset, (UINT8) ((Count >> 8) & 0xff));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
IsaIoUnmap (
  IN EFI_ISA_IO_PROTOCOL                  *This,
  IN VOID                                 *Mapping
  )
/*++

Routine Description:

  Unmaps a memory region for DMA

Arguments:

  This             - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Mapping          - The mapping value returned from EFI_ISA_IO.Map().

Returns:

  EFI_SUCCESS      - The range was unmapped.
  EFI_DEVICE_ERROR - The data was not committed to the target system memory.

--*/
{
  ISA_MAP_INFO  *IsaMapInfo;

  //
  // Unset Feature Flag PcdIsaBusSupportDma to disable support for ISA DMA.
  //
  if (!FeaturePcdGet (PcdIsaBusSupportDma)) {
    return EFI_UNSUPPORTED;
  }

  //
  // See if the Map() operation associated with this Unmap() required a mapping
  // buffer.If a mapping buffer was not required, then this function simply
  // returns EFI_SUCCESS.
  //
  if (Mapping != NULL) {
    //
    // Get the MAP_INFO structure from Mapping
    //
    IsaMapInfo = (ISA_MAP_INFO *) Mapping;

    //
    // If this is a write operation from the Agent's point of view,
    // then copy the contents of the mapped buffer into the real buffer
    // so the processor can read the contents of the real buffer.
    //
    if (IsaMapInfo->Operation == EfiIsaIoOperationBusMasterWrite) {
      CopyMem (
        (VOID *) (UINTN) IsaMapInfo->HostAddress,
        (VOID *) (UINTN) IsaMapInfo->MappedHostAddress,
        IsaMapInfo->NumberOfBytes
        );
    }
    //
    // Free the mapped buffer and the MAP_INFO structure.
    //
    gBS->FreePages (IsaMapInfo->MappedHostAddress, IsaMapInfo->NumberOfPages);
    gBS->FreePool (IsaMapInfo);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
IsaIoFlush (
  IN EFI_ISA_IO_PROTOCOL                  *This
  )
/*++

Routine Description:

  Flushes a DMA buffer

Arguments:

  This             - A pointer to the EFI_ISA_IO_PROTOCOL instance.

Returns:

  EFI_SUCCESS      - The buffers were flushed.
  EFI_DEVICE_ERROR - The buffers were not flushed due to a hardware error.

--*/
{
  EFI_STATUS    Status;
  ISA_IO_DEVICE *IsaIoDevice;

  IsaIoDevice = ISA_IO_DEVICE_FROM_ISA_IO_THIS (This);

  //
  // Call PciIo->Flush
  //
  Status = IsaIoDevice->PciIo->Flush (IsaIoDevice->PciIo);

  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
  }

  return Status;
}

EFI_STATUS
IsaIoVerifyAccess (
  IN     ISA_IO_DEVICE              *IsaIoDevice,
  IN     ISA_ACCESS_TYPE            Type,
  IN     EFI_ISA_IO_PROTOCOL_WIDTH  Width,
  IN     UINTN                      Count,
  IN OUT UINT32                     *Offset
  )
/*++

Routine Description:

  Verifies access to an ISA device

Arguments:

  IsaIoDevice           - The ISA device to be verified.
  Type                  - The Access type. The input must be either IsaAccessTypeMem or IsaAccessTypeIo.
  Width                 - Signifies the width of the memory operation.
  Count                 - The number of memory operations to perform. 
  Offset                - The offset in ISA memory space to start the memory operation.  
  
Returns:

  EFI_SUCCESS           - Verify success.
  EFI_INVALID_PARAMETER - One of the parameters has an invalid value.
  EFI_UNSUPPORTED       - The device ont support the access type.

--*/
{
  EFI_ISA_ACPI_RESOURCE *Item;
  EFI_STATUS            Status;

  if (Width < EfiIsaIoWidthUint8 ||
      Width >= EfiIsaIoWidthMaximum ||
      Width == EfiIsaIoWidthReserved ||
      Width == EfiIsaIoWidthFifoReserved ||
      Width == EfiIsaIoWidthFillReserved
      ) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If Width is EfiIsaIoWidthFifoUintX then convert to EfiIsaIoWidthUintX
  // If Width is EfiIsaIoWidthFillUintX then convert to EfiIsaIoWidthUintX
  //
  if (Width >= EfiIsaIoWidthFifoUint8 && Width <= EfiIsaIoWidthFifoReserved) {
    Count = 1;
  }

  Width = (EFI_ISA_IO_PROTOCOL_WIDTH) (Width & 0x03);

  Status  = EFI_UNSUPPORTED;
  Item    = IsaIoDevice->IsaIo.ResourceList->ResourceItem;
  while (Item->Type != EfiIsaAcpiResourceEndOfList) {
    if ((Type == IsaAccessTypeMem && Item->Type == EfiIsaAcpiResourceMemory) ||
        (Type == IsaAccessTypeIo && Item->Type == EfiIsaAcpiResourceIo)
        ) {
      if (*Offset >= Item->StartRange && (*Offset + Count * (UINT32)(1 << Width)) - 1 <= Item->EndRange) {
        return EFI_SUCCESS;
      }

      if (*Offset >= Item->StartRange && *Offset <= Item->EndRange) {
        Status = EFI_INVALID_PARAMETER;
      }
    }

    Item++;
  }

  return Status;
}

EFI_STATUS
EFIAPI
IsaIoMemRead (
  IN     EFI_ISA_IO_PROTOCOL                       *This,
  IN     EFI_ISA_IO_PROTOCOL_WIDTH                 Width,
  IN     UINT32                                    Offset,
  IN     UINTN                                     Count,
  IN OUT VOID                                      *Buffer
  )
/*++

Routine Description:

  Performs an ISA Memory Read Cycle

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Width                 - Signifies the width of the memory operation.
  Offset                - The offset in ISA memory space to start the memory operation.  
  Count                 - The number of memory operations to perform. 
  Buffer                - The destination buffer to store the results
  
Returns:

  EFI_SUCCESS           - The data was read from the device successfully.
  EFI_UNSUPPORTED       - The Offset is not valid for this device.
  EFI_INVALID_PARAMETER - Width or Count, or both, were invalid.
  EFI_OUT_OF_RESOURCES  - The request could not be completed due to a lack of resources.

--*/
{
  EFI_STATUS    Status;
  ISA_IO_DEVICE *IsaIoDevice;

  //
  // Set Feature Flag PcdIsaBusSupportBusMaster to FALSE to disable support for 
  // ISA Bus Master.
  //
  // So we just return EFI_UNSUPPORTED for these functions.
  //
  if (!FeaturePcdGet (PcdIsaBusSupportIsaMemory)) {
    return EFI_UNSUPPORTED;
  }

  IsaIoDevice = ISA_IO_DEVICE_FROM_ISA_IO_THIS (This);

  //
  // Verify the Isa Io Access
  //
  Status = IsaIoVerifyAccess (
             IsaIoDevice,
             IsaAccessTypeMem,
             Width,
             Count,
             &Offset
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Call PciIo->Mem.Read
  //
  Status = IsaIoDevice->PciIo->Mem.Read (
                                     IsaIoDevice->PciIo,
                                     (EFI_PCI_IO_PROTOCOL_WIDTH) Width,
                                     EFI_PCI_IO_PASS_THROUGH_BAR,
                                     Offset,
                                     Count,
                                     Buffer
                                     );

  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
  }

  return Status;
}

EFI_STATUS
EFIAPI
IsaIoMemWrite (
  IN     EFI_ISA_IO_PROTOCOL                        *This,
  IN     EFI_ISA_IO_PROTOCOL_WIDTH                  Width,
  IN     UINT32                                     Offset,
  IN     UINTN                                      Count,
  IN OUT VOID                                       *Buffer
  )
/*++

Routine Description:

  Performs an ISA Memory Write Cycle

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.  
  Width                 - Signifies the width of the memory operation.
  Offset                - The offset in ISA memory space to start the memory operation.  
  Count                 - The number of memory operations to perform. 
  Buffer                - The source buffer to write data from

Returns:

  EFI_SUCCESS           - The data was written to the device sucessfully.
  EFI_UNSUPPORTED       - The Offset is not valid for this device.
  EFI_INVALID_PARAMETER - Width or Count, or both, were invalid.
  EFI_OUT_OF_RESOURCES  - The request could not be completed due to a lack of resources.

--*/
{
  EFI_STATUS    Status;
  ISA_IO_DEVICE *IsaIoDevice;

  //
  // Set Feature Flag PcdIsaBusSupportBusMaster to FALSE to disable support for 
  // ISA Bus Master.
  //
  // So we just return EFI_UNSUPPORTED for these functions.
  //
  if (!FeaturePcdGet (PcdIsaBusSupportIsaMemory)) {
    return EFI_UNSUPPORTED;
  }

  IsaIoDevice = ISA_IO_DEVICE_FROM_ISA_IO_THIS (This);

  //
  // Verify Isa IO Access
  //
  Status = IsaIoVerifyAccess (
             IsaIoDevice,
             IsaAccessTypeMem,
             Width,
             Count,
             &Offset
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Call PciIo->Mem.Write
  //
  Status = IsaIoDevice->PciIo->Mem.Write (
                                     IsaIoDevice->PciIo,
                                     (EFI_PCI_IO_PROTOCOL_WIDTH) Width,
                                     EFI_PCI_IO_PASS_THROUGH_BAR,
                                     Offset,
                                     Count,
                                     Buffer
                                     );

  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
  }

  return Status;
}

EFI_STATUS
EFIAPI
IsaIoCopyMem (
  IN EFI_ISA_IO_PROTOCOL                        *This,
  IN EFI_ISA_IO_PROTOCOL_WIDTH                  Width,
  IN UINT32                                     DestOffset,
  IN UINT32                                     SrcOffset,
  IN UINTN                                      Count
  )
/*++

Routine Description:

  Performs an ISA I/O Copy Memory 

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Width                 - Signifies the width of the memory copy operation.
  DestOffset            - The offset of the destination 
  SrcOffset             - The offset of the source
  Count                 - The number of memory copy  operations to perform

Returns:

  EFI_SUCCESS           - The data was copied sucessfully.
  EFI_UNSUPPORTED       - The DestOffset or SrcOffset is not valid for this device.
  EFI_INVALID_PARAMETER - Width or Count, or both, were invalid.
  EFI_OUT_OF_RESOURCES  - The request could not be completed due to a lack of resources.

--*/
{
  EFI_STATUS    Status;
  ISA_IO_DEVICE *IsaIoDevice;

  //
  // Set Feature Flag PcdIsaBusSupportBusMaster to FALSE to disable support for 
  // ISA Bus Master.
  //
  // So we just return EFI_UNSUPPORTED for these functions.
  //
  if (!FeaturePcdGet (PcdIsaBusSupportIsaMemory)) {
    return EFI_UNSUPPORTED;
  }

  IsaIoDevice = ISA_IO_DEVICE_FROM_ISA_IO_THIS (This);

  //
  // Verify Isa IO Access for destination and source
  //
  Status = IsaIoVerifyAccess (
             IsaIoDevice,
             IsaAccessTypeMem,
             Width,
             Count,
             &DestOffset
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = IsaIoVerifyAccess (
             IsaIoDevice,
             IsaAccessTypeMem,
             Width,
             Count,
             &SrcOffset
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Call PciIo->CopyMem
  //
  Status = IsaIoDevice->PciIo->CopyMem (
                                 IsaIoDevice->PciIo,
                                 (EFI_PCI_IO_PROTOCOL_WIDTH) Width,
                                 EFI_PCI_IO_PASS_THROUGH_BAR,
                                 DestOffset,
                                 EFI_PCI_IO_PASS_THROUGH_BAR,
                                 SrcOffset,
                                 Count
                                 );

  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
  }

  return Status;
}

STATIC
EFI_STATUS
IsaIoMap_OnlySupportSlaveReadWrite (
  IN     EFI_ISA_IO_PROTOCOL                                  *This,
  IN     EFI_ISA_IO_PROTOCOL_OPERATION                        Operation,
  IN     UINT8                                                ChannelNumber         OPTIONAL,
  IN     UINT32                                               ChannelAttributes,
  IN     VOID                                                 *HostAddress,
  IN OUT UINTN                                                *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                                 *DeviceAddress,
  OUT    VOID                                                 **Mapping
  )
/*++

Routine Description:

  Maps a memory region for DMA, note this implementation
  only supports slave read/write operation to save code size.

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Operation             - Indicates the type of DMA (slave or bus master), and if 
                          the DMA operation is going to read or write to system memory. 
  ChannelNumber         - The slave channel number to use for this DMA operation. 
                          If Operation and ChannelAttributes shows that this device 
                          performs bus mastering DMA, then this field is ignored.  
                          The legal range for this field is 0..7.  
  ChannelAttributes     - The attributes of the DMA channel to use for this DMA operation
  HostAddress           - The system memory address to map to the device.  
  NumberOfBytes         - On input the number of bytes to map.  On output the number 
                          of bytes that were mapped.
  DeviceAddress         - The resulting map address for the bus master device to use 
                          to access the hosts HostAddress.  
  Mapping               - A resulting value to pass to EFI_ISA_IO.Unmap().

Returns:

  EFI_SUCCESS           - The range was mapped for the returned NumberOfBytes.
  EFI_INVALID_PARAMETER - The Operation or HostAddress is undefined.
  EFI_UNSUPPORTED       - The HostAddress can not be mapped as a common buffer.
  EFI_DEVICE_ERROR      - The system hardware could not map the requested address.
  EFI_OUT_OF_RESOURCES  - The memory pages could not be allocated.

--*/
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  ISA_MAP_INFO          *IsaMapInfo;
  UINT8                 DmaMode;
  UINTN                 MaxNumberOfBytes;
  UINT32                BaseAddress;
  UINT16                Count;

  UINT8                 DmaMask;
  UINT8                 DmaClear;
  UINT8                 DmaChannelMode;
  
  if ((NULL == This) ||
      (NULL == HostAddress) ||
      (NULL == NumberOfBytes) ||
      (NULL == DeviceAddress) ||
      (NULL == Mapping)
      ) {
    return EFI_INVALID_PARAMETER;
  }


  //
  // Initialize the return values to their defaults
  //
  *Mapping = NULL;

  //
  // Make sure the Operation parameter is valid.
  // Light IsaIo only supports two operations.
  //
  if (!(Operation == EfiIsaIoOperationSlaveRead || 
        Operation == EfiIsaIoOperationSlaveWrite)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ChannelNumber >= 4) {
    //
    // The Light IsaIo doesn't support channelNumber larger than 4.
    //
    return EFI_INVALID_PARAMETER;
  }

  //
  // Map the HostAddress to a DeviceAddress.
  //
  PhysicalAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) HostAddress;
  if ((PhysicalAddress +*NumberOfBytes) > ISA_MAX_MEMORY_ADDRESS) {
    //
    // Common Buffer operations can not be remapped.  If the common buffer
    // is above 16MB, then it is not possible to generate a mapping, so return
    // an error.
    //
    if (Operation == EfiIsaIoOperationBusMasterCommonBuffer) {
      return EFI_UNSUPPORTED;
    }
    //
    // Allocate an ISA_MAP_INFO structure to remember the mapping when Unmap()
    // is called later.
    //
    IsaMapInfo = AllocatePool (sizeof (ISA_MAP_INFO));
    if (IsaMapInfo == NULL) {
      *NumberOfBytes = 0;
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Return a pointer to the MAP_INFO structure in Mapping
    //
    *Mapping = IsaMapInfo;

    //
    // Initialize the MAP_INFO structure
    //
    IsaMapInfo->Operation         = Operation;
    IsaMapInfo->NumberOfBytes     = *NumberOfBytes;
    IsaMapInfo->NumberOfPages     = EFI_SIZE_TO_PAGES (*NumberOfBytes);
    IsaMapInfo->HostAddress       = PhysicalAddress;
    IsaMapInfo->MappedHostAddress = ISA_MAX_MEMORY_ADDRESS - 1;

    //
    // Allocate a buffer below 16MB to map the transfer to.
    //
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesData,
                    IsaMapInfo->NumberOfPages,
                    &IsaMapInfo->MappedHostAddress
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (IsaMapInfo);
      *NumberOfBytes  = 0;
      *Mapping        = NULL;
      return Status;
    }
    //
    // If this is a read operation from the DMA agents's point of view,
    // then copy the contents of the real buffer into the mapped buffer
    // so the DMA agent can read the contents of the real buffer.
    //
    if (Operation == EfiIsaIoOperationSlaveRead) {
      CopyMem (
        (VOID *) (UINTN) IsaMapInfo->MappedHostAddress,
        (VOID *) (UINTN) IsaMapInfo->HostAddress,
        IsaMapInfo->NumberOfBytes
        );
    }
    //
    // The DeviceAddress is the address of the maped buffer below 16 MB
    //
    *DeviceAddress = IsaMapInfo->MappedHostAddress;
  } else {
    //
    // The transfer is below 16 MB, so the DeviceAddress is simply the
    // HostAddress
    //
    *DeviceAddress = PhysicalAddress;
  }
  
  //
  // Figure out what to program into the DMA Channel Mode Register
  //
  DmaMode = (UINT8) (B_8237_DMA_CHMODE_INCREMENT | (ChannelNumber & 0x03));
  if (Operation == EfiIsaIoOperationSlaveRead) {
    DmaMode |= V_8237_DMA_CHMODE_MEM2IO;
  } else {
    DmaMode |= V_8237_DMA_CHMODE_IO2MEM;
  }
  //
  // We only support EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SINGLE_MODE in simplified IsaIo
  //
  DmaMode |= V_8237_DMA_CHMODE_SINGLE;

  //
  // A Slave DMA transfer can not cross a 64K boundary.
  // Compute *NumberOfBytes based on this restriction.
  //
  MaxNumberOfBytes = 0x10000 - ((UINT32) (*DeviceAddress) & 0xffff);
  if (*NumberOfBytes > MaxNumberOfBytes) {
    *NumberOfBytes = MaxNumberOfBytes;
  }
  //
  // Compute the values to program into the BaseAddress and Count registers
  // of the Slave DMA controller
  //
  BaseAddress = (UINT32) (*DeviceAddress);
  Count       = (UINT16) (*NumberOfBytes - 1);
  //
  // Program the DMA Write Single Mask Register for ChannelNumber
  // Clear the DMA Byte Pointer Register
  //
  DmaMask         = R_8237_DMA_WRSMSK_CH0_3;
  DmaClear        = R_8237_DMA_CBPR_CH0_3;
  DmaChannelMode  = R_8237_DMA_CHMODE_CH0_3;

  Status = WritePort (
             This,
             DmaMask,
             (UINT8) (B_8237_DMA_WRSMSK_CMS | (ChannelNumber & 0x03))
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (
             This,
             DmaClear,
             (UINT8) (B_8237_DMA_WRSMSK_CMS | (ChannelNumber & 0x03))
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (This, DmaChannelMode, DmaMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WriteDmaPort (
             This,
             DmaRegisters[ChannelNumber].Address,
             DmaRegisters[ChannelNumber].Page,
             DmaRegisters[ChannelNumber].Count,
             BaseAddress,
             Count
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (
             This,
             DmaMask,
             (UINT8) (ChannelNumber & 0x03)
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
IsaIoMap_FullSupport (
  IN     EFI_ISA_IO_PROTOCOL                                  *This,
  IN     EFI_ISA_IO_PROTOCOL_OPERATION                        Operation,
  IN     UINT8                                                ChannelNumber         OPTIONAL,
  IN     UINT32                                               ChannelAttributes,
  IN     VOID                                                 *HostAddress,
  IN OUT UINTN                                                *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                                 *DeviceAddress,
  OUT    VOID                                                 **Mapping
  )
/*++

Routine Description:

  Maps a memory region for DMA. This implementation implement the 
  the full mapping support.

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Operation             - Indicates the type of DMA (slave or bus master), and if 
                          the DMA operation is going to read or write to system memory. 
  ChannelNumber         - The slave channel number to use for this DMA operation. 
                          If Operation and ChannelAttributes shows that this device 
                          performs bus mastering DMA, then this field is ignored.  
                          The legal range for this field is 0..7.  
  ChannelAttributes     - The attributes of the DMA channel to use for this DMA operation
  HostAddress           - The system memory address to map to the device.  
  NumberOfBytes         - On input the number of bytes to map.  On output the number 
                          of bytes that were mapped.
  DeviceAddress         - The resulting map address for the bus master device to use 
                        - to access the hosts HostAddress.  
  Mapping               - A resulting value to pass to EFI_ISA_IO.Unmap().

Returns:

  EFI_SUCCESS           - The range was mapped for the returned NumberOfBytes.
  EFI_INVALID_PARAMETER - The Operation or HostAddress is undefined.
  EFI_UNSUPPORTED       - The HostAddress can not be mapped as a common buffer.
  EFI_DEVICE_ERROR      - The system hardware could not map the requested address.
  EFI_OUT_OF_RESOURCES  - The memory pages could not be allocated.

--*/
{
  EFI_STATUS            Status;
  BOOLEAN               Master;
  BOOLEAN               Read;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  ISA_MAP_INFO          *IsaMapInfo;
  UINT8                 DmaMode;
  UINTN                 MaxNumberOfBytes;
  UINT32                BaseAddress;
  UINT16                Count;

  UINT8                 DmaMask;
  UINT8                 DmaClear;
  UINT8                 DmaChannelMode;

  if ((NULL == This) ||
      (NULL == HostAddress) ||
      (NULL == NumberOfBytes) ||
      (NULL == DeviceAddress) ||
      (NULL == Mapping)
      ) {
    return EFI_INVALID_PARAMETER;
  }


  //
  // Initialize the return values to their defaults
  //
  *Mapping = NULL;

  //
  // Make sure the Operation parameter is valid
  //
  if (Operation < 0 || Operation >= EfiIsaIoOperationMaximum) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // See if this is a Slave DMA Operation
  //
  Master  = TRUE;
  Read    = FALSE;
  if (Operation == EfiIsaIoOperationSlaveRead) {
    Operation = EfiIsaIoOperationBusMasterRead;
    Master    = FALSE;
    Read      = TRUE;
  }

  if (Operation == EfiIsaIoOperationSlaveWrite) {
    Operation = EfiIsaIoOperationBusMasterWrite;
    Master    = FALSE;
    Read      = FALSE;
  }

  if (!Master) {
    //
    // Make sure that ChannelNumber is a valid channel number
    // Channel 4 is used to cascade, so it is illegal.
    //
    if (ChannelNumber == 4 || ChannelNumber > 7) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // This implementation only support COMPATIBLE DMA Transfers
    //
    if (!(ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SPEED_COMPATIBLE)) {
      return EFI_INVALID_PARAMETER;
    }

    if (ChannelAttributes &
       (
         EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SPEED_A |
         EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SPEED_B |
         EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SPEED_C
       )
       ) {
      return EFI_INVALID_PARAMETER;
    }

    if (ChannelNumber < 4) {
      //
      // If this is Channel 0..3, then the width must be 8 bit
      //
      if (!(ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_WIDTH_8) ||
          (ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_WIDTH_16)
          ) {
        return EFI_INVALID_PARAMETER;
      }
    } else {
      //
      // If this is Channel 4..7, then the width must be 16 bit
      //
      if ((ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_WIDTH_8) ||
          (!(ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_WIDTH_16))
          ) {
        return EFI_INVALID_PARAMETER;
      }
    }
    //
    // Either Demand Mode or Single Mode must be selected, but not both
    //
    if (ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SINGLE_MODE) {
      if (ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_DEMAND_MODE) {
        return EFI_INVALID_PARAMETER;
      }
    } else {
      if (!(ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_DEMAND_MODE)) {
        return EFI_INVALID_PARAMETER;
      }
    }
  }
  //
  // Map the HostAddress to a DeviceAddress.
  //
  PhysicalAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) HostAddress;
  if ((PhysicalAddress +*NumberOfBytes) > ISA_MAX_MEMORY_ADDRESS) {
    //
    // Common Buffer operations can not be remapped.  If the common buffer
    // is above 16MB, then it is not possible to generate a mapping, so return
    // an error.
    //
    if (Operation == EfiIsaIoOperationBusMasterCommonBuffer) {
      return EFI_UNSUPPORTED;
    }
    //
    // Allocate an ISA_MAP_INFO structure to remember the mapping when Unmap()
    // is called later.
    //
    IsaMapInfo = AllocatePool (sizeof (ISA_MAP_INFO));
    if (IsaMapInfo == NULL) {
      *NumberOfBytes = 0;
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Return a pointer to the MAP_INFO structure in Mapping
    //
    *Mapping = IsaMapInfo;

    //
    // Initialize the MAP_INFO structure
    //
    IsaMapInfo->Operation         = Operation;
    IsaMapInfo->NumberOfBytes     = *NumberOfBytes;
    IsaMapInfo->NumberOfPages     = EFI_SIZE_TO_PAGES (*NumberOfBytes);
    IsaMapInfo->HostAddress       = PhysicalAddress;
    IsaMapInfo->MappedHostAddress = ISA_MAX_MEMORY_ADDRESS - 1;

    //
    // Allocate a buffer below 16MB to map the transfer to.
    //
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesData,
                    IsaMapInfo->NumberOfPages,
                    &IsaMapInfo->MappedHostAddress
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (IsaMapInfo);
      *NumberOfBytes  = 0;
      *Mapping        = NULL;
      return Status;
    }
    //
    // If this is a read operation from the DMA agents's point of view,
    // then copy the contents of the real buffer into the mapped buffer
    // so the DMA agent can read the contents of the real buffer.
    //
    if (Operation == EfiIsaIoOperationBusMasterRead) {
      CopyMem (
        (VOID *) (UINTN) IsaMapInfo->MappedHostAddress,
        (VOID *) (UINTN) IsaMapInfo->HostAddress,
        IsaMapInfo->NumberOfBytes
        );
    }
    //
    // The DeviceAddress is the address of the maped buffer below 16 MB
    //
    *DeviceAddress = IsaMapInfo->MappedHostAddress;
  } else {
    //
    // The transfer is below 16 MB, so the DeviceAddress is simply the
    // HostAddress
    //
    *DeviceAddress = PhysicalAddress;
  }
  //
  // If this is a Bus Master operation then return
  //
  if (Master) {
    return EFI_SUCCESS;
  }
  //
  // Figure out what to program into the DMA Channel Mode Register
  //
  DmaMode = (UINT8) (B_8237_DMA_CHMODE_INCREMENT | (ChannelNumber & 0x03));
  if (Read) {
    DmaMode |= V_8237_DMA_CHMODE_MEM2IO;
  } else {
    DmaMode |= V_8237_DMA_CHMODE_IO2MEM;
  }

  if (ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_AUTO_INITIALIZE) {
    DmaMode |= B_8237_DMA_CHMODE_AE;
  }

  if (ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_DEMAND_MODE) {
    DmaMode |= V_8237_DMA_CHMODE_DEMAND;
  }

  if (ChannelAttributes & EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SINGLE_MODE) {
    DmaMode |= V_8237_DMA_CHMODE_SINGLE;
  }
  //
  // A Slave DMA transfer can not cross a 64K boundary.
  // Compute *NumberOfBytes based on this restriction.
  //
  MaxNumberOfBytes = 0x10000 - ((UINT32) (*DeviceAddress) & 0xffff);
  if (*NumberOfBytes > MaxNumberOfBytes) {
    *NumberOfBytes = MaxNumberOfBytes;
  }
  //
  // Compute the values to program into the BaseAddress and Count registers
  // of the Slave DMA controller
  //
  if (ChannelNumber < 4) {
    BaseAddress = (UINT32) (*DeviceAddress);
    Count       = (UINT16) (*NumberOfBytes - 1);
  } else {
    BaseAddress = (UINT32) (((UINT32) (*DeviceAddress) & 0xff0000) | (((UINT32) (*DeviceAddress) & 0xffff) >> 1));
    Count       = (UINT16) ((*NumberOfBytes - 1) >> 1);
  }
  //
  // Program the DMA Write Single Mask Register for ChannelNumber
  // Clear the DMA Byte Pointer Register
  //
  if (ChannelNumber < 4) {
    DmaMask         = R_8237_DMA_WRSMSK_CH0_3;
    DmaClear        = R_8237_DMA_CBPR_CH0_3;
    DmaChannelMode  = R_8237_DMA_CHMODE_CH0_3;
  } else {
    DmaMask         = R_8237_DMA_WRSMSK_CH4_7;
    DmaClear        = R_8237_DMA_CBPR_CH4_7;
    DmaChannelMode  = R_8237_DMA_CHMODE_CH4_7;
  }

  Status = WritePort (
             This,
             DmaMask,
             (UINT8) (B_8237_DMA_WRSMSK_CMS | (ChannelNumber & 0x03))
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (
             This,
             DmaClear,
             (UINT8) (B_8237_DMA_WRSMSK_CMS | (ChannelNumber & 0x03))
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (This, DmaChannelMode, DmaMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WriteDmaPort (
             This,
             DmaRegisters[ChannelNumber].Address,
             DmaRegisters[ChannelNumber].Page,
             DmaRegisters[ChannelNumber].Count,
             BaseAddress,
             Count
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WritePort (
             This,
             DmaMask,
             (UINT8) (ChannelNumber & 0x03)
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
IsaIoMap (
  IN     EFI_ISA_IO_PROTOCOL                                  *This,
  IN     EFI_ISA_IO_PROTOCOL_OPERATION                        Operation,
  IN     UINT8                                                ChannelNumber         OPTIONAL,
  IN     UINT32                                               ChannelAttributes,
  IN     VOID                                                 *HostAddress,
  IN OUT UINTN                                                *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                                 *DeviceAddress,
  OUT    VOID                                                 **Mapping
  )
/*++

Routine Description:

  Maps a memory region for DMA

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Operation             - Indicates the type of DMA (slave or bus master), and if 
                          the DMA operation is going to read or write to system memory. 
  ChannelNumber         - The slave channel number to use for this DMA operation. 
                          If Operation and ChannelAttributes shows that this device 
                          performs bus mastering DMA, then this field is ignored.  
                          The legal range for this field is 0..7.  
  ChannelAttributes     - The attributes of the DMA channel to use for this DMA operation
  HostAddress           - The system memory address to map to the device.  
  NumberOfBytes         - On input the number of bytes to map.  On output the number 
                          of bytes that were mapped.
  DeviceAddress         - The resulting map address for the bus master device to use 
                        - to access the hosts HostAddress.  
  Mapping               - A resulting value to pass to EFI_ISA_IO.Unmap().

Returns:

  EFI_SUCCESS           - The range was mapped for the returned NumberOfBytes.
  EFI_INVALID_PARAMETER - The Operation or HostAddress is undefined.
  EFI_UNSUPPORTED       - The HostAddress can not be mapped as a common buffer.
  EFI_DEVICE_ERROR      - The system hardware could not map the requested address.
  EFI_OUT_OF_RESOURCES  - The memory pages could not be allocated.

--*/
{
  //
  // Or unset Feature Flag PcdIsaBusSupportDma to disable support for ISA DMA.
  //
  if (!FeaturePcdGet (PcdIsaBusSupportDma)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Set Feature Flag PcdIsaBusSupportBusMaster to FALSE to disable support for 
  // ISA Bus Master.
  //
  // So we just return EFI_UNSUPPORTED for these functions.
  //
  if (FeaturePcdGet (PcdIsaBusOnlySupportSlaveDma)) {
    return IsaIoMap_OnlySupportSlaveReadWrite (
              This,
              Operation,
              ChannelNumber,
              ChannelAttributes,
              HostAddress,
              NumberOfBytes,
              DeviceAddress,
              Mapping
             );

  } else {
    return IsaIoMap_FullSupport (
              This,
              Operation,
              ChannelNumber,
              ChannelAttributes,
              HostAddress,
              NumberOfBytes,
              DeviceAddress,
              Mapping
             );
  }
}
EFI_STATUS
EFIAPI
IsaIoAllocateBuffer (
  IN  EFI_ISA_IO_PROTOCOL                  *This,
  IN  EFI_ALLOCATE_TYPE                    Type,
  IN  EFI_MEMORY_TYPE                      MemoryType,
  IN  UINTN                                Pages,
  OUT VOID                                 **HostAddress,
  IN  UINT64                               Attributes
  )
/*++

Routine Description:

  Allocates a common buffer for DMA

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Type                  - The type allocation to perform.
  MemoryType            - The type of memory to allocate.
  Pages                 - The number of pages to allocate.
  HostAddress           - A pointer to store the base address of the allocated range.
  Attributes            - The requested bit mask of attributes for the allocated range.

Returns:

  EFI_SUCCESS           - The requested memory pages were allocated.
  EFI_INVALID_PARAMETER - Type is invalid or MemoryType is invalid or HostAddress is NULL
  EFI_UNSUPPORTED       - Attributes is unsupported or the memory range specified 
                          by HostAddress, Pages, and Type is not available for common buffer use.
  EFI_OUT_OF_RESOURCES  - The memory pages could not be allocated.

--*/
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;

  //
  // Set Feature Flag PcdIsaBusOnlySupportSlaveDma to FALSE to disable support for 
  // ISA Bus Master.
  // Or unset Feature Flag PcdIsaBusSupportDma to disable support for ISA DMA.
  //
  if (!FeaturePcdGet (PcdIsaBusSupportDma) || FeaturePcdGet (PcdIsaBusOnlySupportSlaveDma)) {
    return EFI_UNSUPPORTED;
  }

  if (HostAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Type < AllocateAnyPages || Type >= MaxAllocateType) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // The only valid memory types are EfiBootServicesData and EfiRuntimeServicesData
  //
  if (MemoryType != EfiBootServicesData && MemoryType != EfiRuntimeServicesData) {
    return EFI_INVALID_PARAMETER;
  }

  if (Attributes &~(EFI_ISA_IO_ATTRIBUTE_MEMORY_WRITE_COMBINE | EFI_ISA_IO_ATTRIBUTE_MEMORY_CACHED)) {
    return EFI_UNSUPPORTED;
  }

  PhysicalAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) (ISA_MAX_MEMORY_ADDRESS - 1);
  if (Type == AllocateAddress) {
    if ((UINTN) (*HostAddress) >= ISA_MAX_MEMORY_ADDRESS) {
      return EFI_UNSUPPORTED;
    } else {
      PhysicalAddress = (UINTN) (*HostAddress);
    }
  }

  if (Type == AllocateAnyPages) {
    Type = AllocateMaxAddress;
  }

  Status = gBS->AllocatePages (Type, MemoryType, Pages, &PhysicalAddress);
  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
    return Status;
  }

  *HostAddress = (VOID *) (UINTN) PhysicalAddress;
  return Status;
}

EFI_STATUS
EFIAPI
IsaIoFreeBuffer (
  IN EFI_ISA_IO_PROTOCOL                  *This,
  IN UINTN                                Pages,
  IN VOID                                 *HostAddress
  )
/*++

Routine Description:

  Frees a common buffer 

Arguments:

  This                  - A pointer to the EFI_ISA_IO_PROTOCOL instance.
  Pages                 - The number of pages to free.
  HostAddress           - The base address of the allocated range.

Returns:

  EFI_SUCCESS           - The requested memory pages were freed.
  EFI_INVALID_PARAMETER - The memory was not allocated with EFI_ISA_IO.AllocateBufer().

--*/
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;

  //
  // Set Feature Flag PcdIsaBusOnlySupportSlaveDma to FALSE to disable support for 
  // ISA Bus Master.
  // Or unset Feature Flag PcdIsaBusSupportDma to disable support for ISA DMA.
  //
  if (!FeaturePcdGet (PcdIsaBusSupportDma) || FeaturePcdGet (PcdIsaBusOnlySupportSlaveDma)) {
    return EFI_UNSUPPORTED;
  }

  PhysicalAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) HostAddress;
  Status = gBS->FreePages (
                  PhysicalAddress,
                  Pages
                  );
  if (EFI_ERROR (Status)) {
    ReportErrorStatusCode (EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR);
  }

  return Status;
}

