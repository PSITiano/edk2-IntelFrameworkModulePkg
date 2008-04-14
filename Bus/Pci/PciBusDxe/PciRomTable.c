/**@file
  Option Rom Support for PCI Bus Driver

Copyright (c) 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include "pcibus.h"
#include "PciRomTable.h"

typedef struct {
  EFI_HANDLE  ImageHandle;
  UINTN       Seg;
  UINT8       Bus;
  UINT8       Dev;
  UINT8       Func;
  UINT64      RomAddress;
  UINT64      RomLength;
} EFI_PCI_ROM_IMAGE_MAPPING;

static UINTN                      mNumberOfPciRomImages     = 0;
static UINTN                      mMaxNumberOfPciRomImages  = 0;
static EFI_PCI_ROM_IMAGE_MAPPING  *mRomImageTable           = NULL;

VOID
PciRomAddImageMapping (
  IN EFI_HANDLE  ImageHandle,
  IN UINTN       Seg,
  IN UINT8       Bus,
  IN UINT8       Dev,
  IN UINT8       Func,
  IN UINT64      RomAddress,
  IN UINT64      RomLength
  )
/**

Routine Description:

  TODO: Add function description

Arguments:

  ImageHandle - TODO: add argument description
  Seg         - TODO: add argument description
  Bus         - TODO: add argument description
  Dev         - TODO: add argument description
  Func        - TODO: add argument description
  RomAddress  - TODO: add argument description
  RomLength   - TODO: add argument description

Returns:

  TODO: add return values

**/
{
  EFI_PCI_ROM_IMAGE_MAPPING *TempMapping;

  if (mNumberOfPciRomImages >= mMaxNumberOfPciRomImages) {

    mMaxNumberOfPciRomImages += 0x20;

    TempMapping = NULL;
    TempMapping = AllocatePool (mMaxNumberOfPciRomImages * sizeof (EFI_PCI_ROM_IMAGE_MAPPING));
    if (TempMapping == NULL) {
      return ;
    }

    CopyMem (TempMapping, mRomImageTable, mNumberOfPciRomImages * sizeof (EFI_PCI_ROM_IMAGE_MAPPING));

    if (mRomImageTable != NULL) {
      gBS->FreePool (mRomImageTable);
    }

    mRomImageTable = TempMapping;
  }

  mRomImageTable[mNumberOfPciRomImages].ImageHandle = ImageHandle;
  mRomImageTable[mNumberOfPciRomImages].Seg         = Seg;
  mRomImageTable[mNumberOfPciRomImages].Bus         = Bus;
  mRomImageTable[mNumberOfPciRomImages].Dev         = Dev;
  mRomImageTable[mNumberOfPciRomImages].Func        = Func;
  mRomImageTable[mNumberOfPciRomImages].RomAddress  = RomAddress;
  mRomImageTable[mNumberOfPciRomImages].RomLength   = RomLength;
  mNumberOfPciRomImages++;
}

EFI_STATUS
PciRomGetRomResourceFromPciOptionRomTable (
  IN EFI_DRIVER_BINDING_PROTOCOL      *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
  PCI_IO_DEVICE                       *PciIoDevice
  )
/**

Routine Description:

Arguments:

Returns:

**/
// TODO:    This - add argument and description to function comment
// TODO:    PciRootBridgeIo - add argument and description to function comment
// TODO:    PciIoDevice - add argument and description to function comment
// TODO:    EFI_NOT_FOUND - add return value to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_STATUS                    Status;
  EFI_PCI_OPTION_ROM_TABLE      *PciOptionRomTable;
  EFI_PCI_OPTION_ROM_DESCRIPTOR *PciOptionRomDescriptor;
  UINTN                         Index;

  Status = EfiGetSystemConfigurationTable (&gEfiPciOptionRomTableGuid, (VOID **) &PciOptionRomTable);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < PciOptionRomTable->PciOptionRomCount; Index++) {
    PciOptionRomDescriptor = &PciOptionRomTable->PciOptionRomDescriptors[Index];
    if (PciOptionRomDescriptor->Seg == PciRootBridgeIo->SegmentNumber &&
        PciOptionRomDescriptor->Bus == PciIoDevice->BusNumber         &&
        PciOptionRomDescriptor->Dev == PciIoDevice->DeviceNumber      &&
        PciOptionRomDescriptor->Func == PciIoDevice->FunctionNumber ) {

      PciIoDevice->PciIo.RomImage = (VOID *) (UINTN) PciOptionRomDescriptor->RomAddress;
      PciIoDevice->PciIo.RomSize  = (UINTN) PciOptionRomDescriptor->RomLength;
    }
  }

  for (Index = 0; Index < mNumberOfPciRomImages; Index++) {
    if (mRomImageTable[Index].Seg  == PciRootBridgeIo->SegmentNumber &&
        mRomImageTable[Index].Bus  == PciIoDevice->BusNumber         &&
        mRomImageTable[Index].Dev  == PciIoDevice->DeviceNumber      &&
        mRomImageTable[Index].Func == PciIoDevice->FunctionNumber    ) {

      AddDriver (PciIoDevice, mRomImageTable[Index].ImageHandle);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciRomGetImageMapping (
  PCI_IO_DEVICE                       *PciIoDevice
  )
/**

Routine Description:

Arguments:

Returns:

**/
// TODO:    PciIoDevice - add argument and description to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINTN                           Index;

  PciRootBridgeIo = PciIoDevice->PciRootBridgeIo;

  for (Index = 0; Index < mNumberOfPciRomImages; Index++) {
    if (mRomImageTable[Index].Seg  == PciRootBridgeIo->SegmentNumber &&
        mRomImageTable[Index].Bus  == PciIoDevice->BusNumber         &&
        mRomImageTable[Index].Dev  == PciIoDevice->DeviceNumber      &&
        mRomImageTable[Index].Func == PciIoDevice->FunctionNumber    ) {

      if (mRomImageTable[Index].ImageHandle != NULL) {
        AddDriver (PciIoDevice, mRomImageTable[Index].ImageHandle);
      } else {
        PciIoDevice->PciIo.RomImage = (VOID *) (UINTN) mRomImageTable[Index].RomAddress;
        PciIoDevice->PciIo.RomSize  = (UINTN) mRomImageTable[Index].RomLength;
      }
    }
  }

  return EFI_SUCCESS;
}
