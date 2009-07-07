/** @file
  Internal library implementation for PCI Bus module.

Copyright (c) 2006 - 2009, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PciBus.h"


/**
  Retrieve the PCI Card device BAR information via PciIo interface.

  @param PciIoDevice        PCI Card device instance.

**/
VOID
GetBackPcCardBar (
  IN  PCI_IO_DEVICE                  *PciIoDevice
  )
{
  UINT32  Address;

  if (!FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
    return;
  }

  //
  // Read PciBar information from the bar register
  //
  if (!gFullEnumeration) {
    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                             &(PciIoDevice->PciIo),
                             EfiPciIoWidthUint32,
                             PCI_CARD_MEMORY_BASE_0,
                             1,
                             &Address
                             );

    (PciIoDevice->PciBar)[P2C_MEM_1].BaseAddress  = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_MEM_1].Length       = 0x2000000;
    (PciIoDevice->PciBar)[P2C_MEM_1].BarType      = PciBarTypeMem32;

    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                             &(PciIoDevice->PciIo),
                             EfiPciIoWidthUint32,
                             PCI_CARD_MEMORY_BASE_1,
                             1,
                             &Address
                             );
    (PciIoDevice->PciBar)[P2C_MEM_2].BaseAddress  = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_MEM_2].Length       = 0x2000000;
    (PciIoDevice->PciBar)[P2C_MEM_2].BarType      = PciBarTypePMem32;

    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                             &(PciIoDevice->PciIo),
                             EfiPciIoWidthUint32,
                             PCI_CARD_IO_BASE_0_LOWER,
                             1,
                             &Address
                             );
    (PciIoDevice->PciBar)[P2C_IO_1].BaseAddress = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_IO_1].Length      = 0x100;
    (PciIoDevice->PciBar)[P2C_IO_1].BarType     = PciBarTypeIo16;

    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                             &(PciIoDevice->PciIo),
                             EfiPciIoWidthUint32,
                             PCI_CARD_IO_BASE_1_LOWER,
                             1,
                             &Address
                             );
    (PciIoDevice->PciBar)[P2C_IO_2].BaseAddress = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_IO_2].Length      = 0x100;
    (PciIoDevice->PciBar)[P2C_IO_2].BarType     = PciBarTypeIo16;

  }

  if (gPciHotPlugInit != NULL && FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
    GetResourcePaddingForHpb (PciIoDevice);
  }
}

/**
  Remove rejected pci device from specific root bridge
  handle.

  @param RootBridgeHandle  Specific parent root bridge handle.
  @param Bridge            Bridge device instance.

**/
VOID
RemoveRejectedPciDevices (
  IN EFI_HANDLE        RootBridgeHandle,
  IN PCI_IO_DEVICE     *Bridge
  )
{
  PCI_IO_DEVICE   *Temp;
  LIST_ENTRY      *CurrentLink;
  LIST_ENTRY      *LastLink;

  if (!FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
    return;
  }

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink != NULL && CurrentLink != &Bridge->ChildList) {

    Temp = PCI_IO_DEVICE_FROM_LINK (CurrentLink);

    if (IS_PCI_BRIDGE (&Temp->Pci)) {
      //
      // Remove rejected devices recusively
      //
      RemoveRejectedPciDevices (RootBridgeHandle, Temp);
    } else {
      //
      // Skip rejection for all PPBs, while detect rejection for others
      //
      if (IsPciDeviceRejected (Temp)) {

        //
        // For P2C, remove all devices on it
        //
        if (!IsListEmpty (&Temp->ChildList)) {
          RemoveAllPciDeviceOnBridge (RootBridgeHandle, Temp);
        }

        //
        // Finally remove itself
        //
        LastLink = CurrentLink->BackLink;
        RemoveEntryList (CurrentLink);
        FreePciDevice (Temp);

        CurrentLink = LastLink;
      }
    }

    CurrentLink = CurrentLink->ForwardLink;
  }
}

/**
  Submits the I/O and memory resource requirements for the specified PCI Host Bridge.

  @param PciResAlloc  Point to protocol instance of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL.

  @retval EFI_SUCCESS           Successfully finished resource allocation.
  @retval EFI_NOT_FOUND         Cannot get root bridge instance.
  @retval EFI_OUT_OF_RESOURCES  Platform failed to program the resources if no hot plug supported.
  @retval other                 Some error occurred when allocating resources for the PCI Host Bridge.

  @note   Feature flag PcdPciBusHotplugDeviceSupport determine whether need support hotplug.

**/
EFI_STATUS
PciHostBridgeResourceAllocator (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *PciResAlloc
  )
{
  PCI_IO_DEVICE                                  *RootBridgeDev;
  EFI_HANDLE                                     RootBridgeHandle;
  VOID                                           *AcpiConfig;
  EFI_STATUS                                     Status;
  UINT64                                         IoBase;
  UINT64                                         Mem32Base;
  UINT64                                         PMem32Base;
  UINT64                                         Mem64Base;
  UINT64                                         PMem64Base;
  UINT64                                         IoResStatus;
  UINT64                                         Mem32ResStatus;
  UINT64                                         PMem32ResStatus;
  UINT64                                         Mem64ResStatus;
  UINT64                                         PMem64ResStatus;
  UINT64                                         MaxOptionRomSize;
  PCI_RESOURCE_NODE                              *IoBridge;
  PCI_RESOURCE_NODE                              *Mem32Bridge;
  PCI_RESOURCE_NODE                              *PMem32Bridge;
  PCI_RESOURCE_NODE                              *Mem64Bridge;
  PCI_RESOURCE_NODE                              *PMem64Bridge;
  PCI_RESOURCE_NODE                              IoPool;
  PCI_RESOURCE_NODE                              Mem32Pool;
  PCI_RESOURCE_NODE                              PMem32Pool;
  PCI_RESOURCE_NODE                              Mem64Pool;
  PCI_RESOURCE_NODE                              PMem64Pool;
  BOOLEAN                                        ReAllocate;
  EFI_DEVICE_HANDLE_EXTENDED_DATA_PAYLOAD        HandleExtendedData;
  EFI_RESOURCE_ALLOC_FAILURE_ERROR_DATA_PAYLOAD  AllocFailExtendedData;

  //
  // Reallocate flag
  //
  ReAllocate = FALSE;

  //
  // It may try several times if the resource allocation fails
  //
  while (TRUE) {
    //
    // Initialize resource pool
    //
    InitializeResourcePool (&IoPool, PciBarTypeIo16);
    InitializeResourcePool (&Mem32Pool, PciBarTypeMem32);
    InitializeResourcePool (&PMem32Pool, PciBarTypePMem32);
    InitializeResourcePool (&Mem64Pool, PciBarTypeMem64);
    InitializeResourcePool (&PMem64Pool, PciBarTypePMem64);

    RootBridgeDev     = NULL;
    RootBridgeHandle  = 0;

    while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {
      //
      // Get Root Bridge Device by handle
      //
      RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);

      if (RootBridgeDev == NULL) {
        return EFI_NOT_FOUND;
      }

      //
      // Create the entire system resource map from the information collected by
      // enumerator. Several resource tree was created
      //

      IoBridge = CreateResourceNode (
                   RootBridgeDev,
                   0,
                   0xFFF,
                   0,
                   PciBarTypeIo16,
                   PciResUsageTypical
                   );

      Mem32Bridge = CreateResourceNode (
                      RootBridgeDev,
                      0,
                      0xFFFFF,
                      0,
                      PciBarTypeMem32,
                      PciResUsageTypical
                      );

      PMem32Bridge = CreateResourceNode (
                       RootBridgeDev,
                       0,
                       0xFFFFF,
                       0,
                       PciBarTypePMem32,
                       PciResUsageTypical
                       );

      Mem64Bridge = CreateResourceNode (
                      RootBridgeDev,
                      0,
                      0xFFFFF,
                      0,
                      PciBarTypeMem64,
                      PciResUsageTypical
                      );

      PMem64Bridge = CreateResourceNode (
                       RootBridgeDev,
                       0,
                       0xFFFFF,
                       0,
                       PciBarTypePMem64,
                       PciResUsageTypical
                       );

      //
      // Create resourcemap by going through all the devices subject to this root bridge
      //
      CreateResourceMap (
        RootBridgeDev,
        IoBridge,
        Mem32Bridge,
        PMem32Bridge,
        Mem64Bridge,
        PMem64Bridge
        );

      //
      // Get the max ROM size that the root bridge can process
      //
      RootBridgeDev->RomSize = Mem32Bridge->Length;

      //
      // Skip to enlarge the resource request during realloction
      //
      if (!ReAllocate) {
        //
        // Get Max Option Rom size for current root bridge
        //
        MaxOptionRomSize = GetMaxOptionRomSize (RootBridgeDev);

        //
        // Enlarger the mem32 resource to accomdate the option rom
        // if the mem32 resource is not enough to hold the rom
        //
        if (MaxOptionRomSize > Mem32Bridge->Length) {

          Mem32Bridge->Length     = MaxOptionRomSize;
          RootBridgeDev->RomSize  = MaxOptionRomSize;

          //
          // Alignment should be adjusted as well
          //
          if (Mem32Bridge->Alignment < MaxOptionRomSize - 1) {
            Mem32Bridge->Alignment = MaxOptionRomSize - 1;
          }
        }
      }

      //
      // Based on the all the resource tree, contruct ACPI resource node to
      // submit the resource aperture to pci host bridge protocol
      //
      Status = ConstructAcpiResourceRequestor (
                 RootBridgeDev,
                 IoBridge,
                 Mem32Bridge,
                 PMem32Bridge,
                 Mem64Bridge,
                 PMem64Bridge,
                 &AcpiConfig
                 );

      //
      // Insert these resource nodes into the database
      //
      InsertResourceNode (&IoPool, IoBridge);
      InsertResourceNode (&Mem32Pool, Mem32Bridge);
      InsertResourceNode (&PMem32Pool, PMem32Bridge);
      InsertResourceNode (&Mem64Pool, Mem64Bridge);
      InsertResourceNode (&PMem64Pool, PMem64Bridge);

      if (Status == EFI_SUCCESS) {
        //
        // Submit the resource requirement
        //
        Status = PciResAlloc->SubmitResources (
                                PciResAlloc,
                                RootBridgeDev->Handle,
                                AcpiConfig
                                );
      }

      //
      // Free acpi resource node
      //
      if (AcpiConfig != NULL) {
        FreePool (AcpiConfig);
      }

      if (EFI_ERROR (Status)) {
        //
        // Destroy all the resource tree
        //
        DestroyResourceTree (&IoPool);
        DestroyResourceTree (&Mem32Pool);
        DestroyResourceTree (&PMem32Pool);
        DestroyResourceTree (&Mem64Pool);
        DestroyResourceTree (&PMem64Pool);
        return Status;
      }
    }
    //
    // End while, at least one Root Bridge should be found.
    //
    ASSERT (RootBridgeDev != NULL);

    //
    // Notify platform to start to program the resource
    //
    Status = NotifyPhase (PciResAlloc, EfiPciHostBridgeAllocateResources);
    if (!FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
      //
      // If Hot Plug is not supported
      //
      if (EFI_ERROR (Status)) {
        //
        // Allocation failed, then return
        //
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // Allocation succeed.
      // Get host bridge handle for status report, and then skip the main while
      //
      HandleExtendedData.Handle = RootBridgeDev->PciRootBridgeIo->ParentHandle;

      break;

    } else {
      //
      // If Hot Plug is supported
      //
      if (!EFI_ERROR (Status)) {
        //
        // Allocation succeed, then continue the following
        //
        break;
      }

      //
      // If the resource allocation is unsuccessful, free resources on bridge
      //

      RootBridgeDev     = NULL;
      RootBridgeHandle  = 0;

      IoResStatus       = EFI_RESOURCE_SATISFIED;
      Mem32ResStatus    = EFI_RESOURCE_SATISFIED;
      PMem32ResStatus   = EFI_RESOURCE_SATISFIED;
      Mem64ResStatus    = EFI_RESOURCE_SATISFIED;
      PMem64ResStatus   = EFI_RESOURCE_SATISFIED;

      while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {
        //
        // Get RootBridg Device by handle
        //
        RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);
        if (RootBridgeDev == NULL) {
          return EFI_NOT_FOUND;
        }

        //
        // Get host bridge handle for status report
        //
        HandleExtendedData.Handle = RootBridgeDev->PciRootBridgeIo->ParentHandle;

        //
        // Get acpi resource node for all the resource types
        //
        AcpiConfig = NULL;

        Status = PciResAlloc->GetProposedResources (
                                PciResAlloc,
                                RootBridgeDev->Handle,
                                &AcpiConfig
                                );

        if (EFI_ERROR (Status)) {
          return Status;
        }

        if (AcpiConfig != NULL) {
          //
          // Adjust resource allocation policy for each RB
          //
          GetResourceAllocationStatus (
            AcpiConfig,
            &IoResStatus,
            &Mem32ResStatus,
            &PMem32ResStatus,
            &Mem64ResStatus,
            &PMem64ResStatus
            );
          FreePool (AcpiConfig);
        }
      }
      //
      // End while
      //

      //
      // Raise the EFI_IOB_EC_RESOURCE_CONFLICT status code
      //
      //
      // It is very difficult to follow the spec here
      // Device path , Bar index can not be get here
      //
      ZeroMem (&AllocFailExtendedData, sizeof (AllocFailExtendedData));

      REPORT_STATUS_CODE_WITH_EXTENDED_DATA (
            EFI_PROGRESS_CODE,
            EFI_IO_BUS_PCI | EFI_IOB_EC_RESOURCE_CONFLICT,
            (VOID *) &AllocFailExtendedData,
            sizeof (AllocFailExtendedData)
            );

      Status = PciHostBridgeAdjustAllocation (
                 &IoPool,
                 &Mem32Pool,
                 &PMem32Pool,
                 &Mem64Pool,
                 &PMem64Pool,
                 IoResStatus,
                 Mem32ResStatus,
                 PMem32ResStatus,
                 Mem64ResStatus,
                 PMem64ResStatus
                 );

      //
      // Destroy all the resource tree
      //
      DestroyResourceTree (&IoPool);
      DestroyResourceTree (&Mem32Pool);
      DestroyResourceTree (&PMem32Pool);
      DestroyResourceTree (&Mem64Pool);
      DestroyResourceTree (&PMem64Pool);

      NotifyPhase (PciResAlloc, EfiPciHostBridgeFreeResources);

      if (EFI_ERROR (Status)) {
        return Status;
      }

      ReAllocate = TRUE;
    }
  }
  //
  // End main while
  //

  //
  // Raise the EFI_IOB_PCI_RES_ALLOC status code
  //
  REPORT_STATUS_CODE_WITH_EXTENDED_DATA (
        EFI_PROGRESS_CODE,
        EFI_IO_BUS_PCI | EFI_IOB_PCI_PC_RES_ALLOC,
        (VOID *) &HandleExtendedData,
        sizeof (HandleExtendedData)
        );

  //
  // Notify pci bus driver starts to program the resource
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeSetResources);

  RootBridgeDev     = NULL;

  RootBridgeHandle  = 0;

  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {
    //
    // Get RootBridg Device by handle
    //
    RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_NOT_FOUND;
    }

    //
    // Get acpi resource node for all the resource types
    //
    AcpiConfig = NULL;
    Status = PciResAlloc->GetProposedResources (
                            PciResAlloc,
                            RootBridgeDev->Handle,
                            &AcpiConfig
                            );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Get the resource base by interpreting acpi resource node
    //
    //
    GetResourceBase (
      AcpiConfig,
      &IoBase,
      &Mem32Base,
      &PMem32Base,
      &Mem64Base,
      &PMem64Base
      );

    //
    // Process option rom for this root bridge
    //
    ProcessOptionRom (RootBridgeDev, Mem32Base, RootBridgeDev->RomSize);

    //
    // Create the entire system resource map from the information collected by
    // enumerator. Several resource tree was created
    //
    GetResourceMap (
      RootBridgeDev,
      &IoBridge,
      &Mem32Bridge,
      &PMem32Bridge,
      &Mem64Bridge,
      &PMem64Bridge,
      &IoPool,
      &Mem32Pool,
      &PMem32Pool,
      &Mem64Pool,
      &PMem64Pool
      );

    //
    // Program IO resources
    //
    ProgramResource (
      IoBase,
      IoBridge
      );

    //
    // Program Mem32 resources
    //
    ProgramResource (
      Mem32Base,
      Mem32Bridge
      );

    //
    // Program PMem32 resources
    //
    ProgramResource (
      PMem32Base,
      PMem32Bridge
      );

    //
    // Program Mem64 resources
    //
    ProgramResource (
      Mem64Base,
      Mem64Bridge
      );

    //
    // Program PMem64 resources
    //
    ProgramResource (
      PMem64Base,
      PMem64Bridge
      );

    FreePool (AcpiConfig);
  }

  //
  // Destroy all the resource tree
  //
  DestroyResourceTree (&IoPool);
  DestroyResourceTree (&Mem32Pool);
  DestroyResourceTree (&PMem32Pool);
  DestroyResourceTree (&Mem64Pool);
  DestroyResourceTree (&PMem64Pool);

  //
  // Notify the resource allocation phase is to end
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeEndResourceAllocation);

  return EFI_SUCCESS;
}

/**
  Scan pci bus and assign bus number to the given PCI bus system.

  @param  Bridge           Bridge device instance.
  @param  StartBusNumber   start point.
  @param  SubBusNumber     Point to sub bus number.
  @param  PaddedBusRange   Customized bus number.

  @retval EFI_SUCCESS      Successfully scanned and assigned bus number.
  @retval other            Some error occurred when scanning pci bus.

  @note   Feature flag PcdPciBusHotplugDeviceSupport determine whether need support hotplug.

**/
EFI_STATUS
PciScanBus (
  IN PCI_IO_DEVICE                      *Bridge,
  IN UINT8                              StartBusNumber,
  OUT UINT8                             *SubBusNumber,
  OUT UINT8                             *PaddedBusRange
  )
{
  EFI_STATUS                        Status;
  PCI_TYPE00                        Pci;
  UINT8                             Device;
  UINT8                             Func;
  UINT64                            Address;
  UINTN                             SecondBus;
  UINT16                            Register;
  UINTN                             HpIndex;
  PCI_IO_DEVICE                     *PciDevice;
  EFI_EVENT                         Event;
  EFI_HPC_STATE                     State;
  UINT64                            PciAddress;
  EFI_HPC_PADDING_ATTRIBUTES        Attributes;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  UINT16                            BusRange;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo;
  BOOLEAN                           BusPadding;

  PciRootBridgeIo = Bridge->PciRootBridgeIo;
  SecondBus       = 0;
  Register        = 0;
  State           = 0;
  Attributes      = (EFI_HPC_PADDING_ATTRIBUTES) 0;
  BusRange        = 0;
  BusPadding      = FALSE;
  PciDevice       = NULL;
  PciAddress      = 0;

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {

      //
      // Check to see whether a pci device is present
      //
      Status = PciDevicePresent (
                PciRootBridgeIo,
                &Pci,
                StartBusNumber,
                Device,
                Func
                );

      if (EFI_ERROR (Status)) {
        if (Func == 0) {
          //
          // Skip sub functions, this is not a multi function device
          //
          Func = PCI_MAX_FUNC;
        }

        continue;
      }

      DEBUG((EFI_D_ERROR, "Found DEV(%02d,%02d,%02d)\n", StartBusNumber, Device, Func ));

      if (FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
        //
        // Get the PCI device information
        //
        Status = PciSearchDevice (
                  Bridge,
                  &Pci,
                  StartBusNumber,
                  Device,
                  Func,
                  &PciDevice
                  );

        ASSERT (!EFI_ERROR (Status));

        PciAddress = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, 0);

        if (!IS_PCI_BRIDGE (&Pci)) {
          //
          // PCI bridges will be called later
          // Here just need for PCI device or PCI to cardbus controller
          // EfiPciBeforeChildBusEnumeration for PCI Device Node
          //
          PreprocessController (
              PciDevice,
              PciDevice->BusNumber,
              PciDevice->DeviceNumber,
              PciDevice->FunctionNumber,
              EfiPciBeforeChildBusEnumeration
              );
        }

        //
        // For Pci Hotplug controller devcie only
        //
        if (gPciHotPlugInit != NULL) {
          //
          // Check if it is a Hotplug PCI controller
          //
          if (IsRootPciHotPlugController (PciDevice->DevicePath, &HpIndex)) {

            if (!gPciRootHpcData[HpIndex].Initialized) {

              Status = CreateEventForHpc (HpIndex, &Event);

              ASSERT (!EFI_ERROR (Status));

              Status = gPciHotPlugInit->InitializeRootHpc (
                                          gPciHotPlugInit,
                                          gPciRootHpcPool[HpIndex].HpcDevicePath,
                                          PciAddress,
                                          Event,
                                          &State
                                          );

              PreprocessController (
                PciDevice,
                PciDevice->BusNumber,
                PciDevice->DeviceNumber,
                PciDevice->FunctionNumber,
                EfiPciBeforeChildBusEnumeration
              );
            }
          }
        }
      }

      if (IS_PCI_BRIDGE (&Pci) || IS_CARDBUS_BRIDGE (&Pci)) {
        //
        // For PPB
        //
        if (!FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
          //
          // If Hot Plug is not supported,
          // get the bridge information
          //
          Status = PciSearchDevice (
                    Bridge,
                    &Pci,
                    StartBusNumber,
                    Device,
                    Func,
                    &PciDevice
                    );

          if (EFI_ERROR (Status)) {
            return Status;
          }
        } else {
          //
          // If Hot Plug is supported,
          // Get the bridge information
          //
          BusPadding = FALSE;
          if (gPciHotPlugInit != NULL) {

            if (IsRootPciHotPlugBus (PciDevice->DevicePath, &HpIndex)) {

              //
              // If it is initialized, get the padded bus range
              //
              Status = gPciHotPlugInit->GetResourcePadding (
                                          gPciHotPlugInit,
                                          gPciRootHpcPool[HpIndex].HpbDevicePath,
                                          PciAddress,
                                          &State,
                                          (VOID **) &Descriptors,
                                          &Attributes
                                          );

              if (EFI_ERROR (Status)) {
                return Status;
              }

              BusRange = 0;
              Status = PciGetBusRange (
                        &Descriptors,
                        NULL,
                        NULL,
                        &BusRange
                        );

              FreePool (Descriptors);

              if (EFI_ERROR (Status)) {
                return Status;
              }

              BusPadding = TRUE;
            }
          }
        }

        //
        // Add feature to support customized secondary bus number
        //
        if (*SubBusNumber == 0) {
          *SubBusNumber   = *PaddedBusRange;
          *PaddedBusRange = 0;
        }

        (*SubBusNumber)++;
        SecondBus = *SubBusNumber;

        Register  = (UINT16) ((SecondBus << 8) | (UINT16) StartBusNumber);
        Address   = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET);

        Status = PciRootBridgeIo->Pci.Write (
                                        PciRootBridgeIo,
                                        EfiPciWidthUint16,
                                        Address,
                                        1,
                                        &Register
                                        );


        //
        // If it is PPB, resursively search down this bridge
        //
        if (IS_PCI_BRIDGE (&Pci)) {

          //
          // Temporarily initialize SubBusNumber to maximum bus number to ensure the
          // PCI configuration transaction to go through any PPB
          //
          Register  = 0xFF;
          Address   = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
          Status = PciRootBridgeIo->Pci.Write (
                                          PciRootBridgeIo,
                                          EfiPciWidthUint8,
                                          Address,
                                          1,
                                          &Register
                                          );

          //
          // Nofify EfiPciBeforeChildBusEnumeration for PCI Brige
          //
          PreprocessController (
            PciDevice,
            PciDevice->BusNumber,
            PciDevice->DeviceNumber,
            PciDevice->FunctionNumber,
            EfiPciBeforeChildBusEnumeration
            );

          DEBUG((EFI_D_ERROR, "Scan  PPB(%02d,%02d,%02d)\n", PciDevice->BusNumber, PciDevice->DeviceNumber,PciDevice->FunctionNumber));
          Status = PciScanBus (
                    PciDevice,
                    (UINT8) (SecondBus),
                    SubBusNumber,
                    PaddedBusRange
                    );
          if (EFI_ERROR (Status)) {
            return Status;
          }
        }

        if (FeaturePcdGet (PcdPciBusHotplugDeviceSupport) && BusPadding) {
          //
          // Ensure the device is enabled and initialized
          //
          if ((Attributes == EfiPaddingPciRootBridge) &&
              (State & EFI_HPC_STATE_ENABLED) != 0    &&
              (State & EFI_HPC_STATE_INITIALIZED) != 0) {
            *PaddedBusRange = (UINT8) ((UINT8) (BusRange) +*PaddedBusRange);
          } else {
            *SubBusNumber = (UINT8) ((UINT8) (BusRange) +*SubBusNumber);
          }
        }

        //
        // Set the current maximum bus number under the PPB
        //
        Address = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);

        Status = PciRootBridgeIo->Pci.Write (
                                        PciRootBridgeIo,
                                        EfiPciWidthUint8,
                                        Address,
                                        1,
                                        SubBusNumber
                                        );
      }

      if (Func == 0 && !IS_PCI_MULTI_FUNC (&Pci)) {

        //
        // Skip sub functions, this is not a multi function device
        //

        Func = PCI_MAX_FUNC;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Process Option Rom on the specified root bridge.

  @param Bridge  Pci root bridge device instance.

  @retval EFI_SUCCESS   Success process.
  @retval other         Some error occurred when processing Option Rom on the root bridge.

**/
EFI_STATUS
PciRootBridgeP2CProcess (
  IN PCI_IO_DEVICE *Bridge
  )
{
  LIST_ENTRY      *CurrentLink;
  PCI_IO_DEVICE   *Temp;
  EFI_HPC_STATE   State;
  UINT64          PciAddress;
  EFI_STATUS      Status;

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink != NULL && CurrentLink != &Bridge->ChildList) {

    Temp = PCI_IO_DEVICE_FROM_LINK (CurrentLink);

    if (IS_CARDBUS_BRIDGE (&Temp->Pci)) {

      if (gPciHotPlugInit != NULL && Temp->Allocated && FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {

        //
        // Raise the EFI_IOB_PCI_HPC_INIT status code
        //
        REPORT_STATUS_CODE_WITH_DEVICE_PATH (
          EFI_PROGRESS_CODE,
          EFI_IO_BUS_PCI | EFI_IOB_PCI_PC_HPC_INIT,
          Temp->DevicePath
          );

        PciAddress = EFI_PCI_ADDRESS (Temp->BusNumber, Temp->DeviceNumber, Temp->FunctionNumber, 0);
        Status = gPciHotPlugInit->InitializeRootHpc (
                                    gPciHotPlugInit,
                                    Temp->DevicePath,
                                    PciAddress,
                                    NULL,
                                    &State
                                    );

        if (!EFI_ERROR (Status)) {
          Status = PciBridgeEnumerator (Temp);

          if (EFI_ERROR (Status)) {
            return Status;
          }
        }

        CurrentLink = CurrentLink->ForwardLink;
        continue;

      }
    }

    if (!IsListEmpty (&Temp->ChildList)) {
      Status = PciRootBridgeP2CProcess (Temp);
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  return EFI_SUCCESS;
}

/**
  Process Option Rom on the specified host bridge.

  @param PciResAlloc    Pointer to instance of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL.

  @retval EFI_SUCCESS   Success process.
  @retval EFI_NOT_FOUND Can not find the root bridge instance.
  @retval other         Some error occurred when processing Option Rom on the host bridge.

**/
EFI_STATUS
PciHostBridgeP2CProcess (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *PciResAlloc
  )
{
  EFI_HANDLE    RootBridgeHandle;
  PCI_IO_DEVICE *RootBridgeDev;
  EFI_STATUS    Status;

  if (!FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
    return EFI_SUCCESS;
  }

  RootBridgeHandle = NULL;

  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

    //
    // Get RootBridg Device by handle
    //
    RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_NOT_FOUND;
    }

    Status = PciRootBridgeP2CProcess (RootBridgeDev);
    if (EFI_ERROR (Status)) {
      return Status;
    }

  }

  return EFI_SUCCESS;
}

/**
  This function is used to enumerate the entire host bridge
  in a given platform.

  @param PciResAlloc   A pointer to the PCI Host Resource Allocation protocol.

  @retval EFI_SUCCESS            Successfully enumerated the host bridge.
  @retval EFI_OUT_OF_RESOURCES   No enough memory available.
  @retval other                  Some error occurred when enumerating the host bridge.

**/
EFI_STATUS
PciHostBridgeEnumerator (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *PciResAlloc
  )
{
  EFI_HANDLE                        RootBridgeHandle;
  PCI_IO_DEVICE                     *RootBridgeDev;
  EFI_STATUS                        Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo;
  UINT16                            MinBus;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Configuration;
  UINT8                             StartBusNumber;
  LIST_ENTRY                        RootBridgeList;
  LIST_ENTRY                        *Link;

  if (FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
    InitializeHotPlugSupport ();
  }

  InitializeListHead (&RootBridgeList);

  //
  // Notify the bus allocation phase is about to start
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeBeginBusAllocation);

  DEBUG((EFI_D_ERROR, "PCI Bus First Scanning\n"));
  RootBridgeHandle = NULL;
  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

    //
    // if a root bridge instance is found, create root bridge device for it
    //

    RootBridgeDev = CreateRootBridge (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Enumerate all the buses under this root bridge
    //
    Status = PciRootBridgeEnumerator (
              PciResAlloc,
              RootBridgeDev
              );

    if (gPciHotPlugInit != NULL && FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
      InsertTailList (&RootBridgeList, &(RootBridgeDev->Link));
    } else {
      DestroyRootBridge (RootBridgeDev);
    }
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Notify the bus allocation phase is finished for the first time
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeEndBusAllocation);

  if (gPciHotPlugInit != NULL && FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
    //
    // Reset all assigned PCI bus number in all PPB
    //
    RootBridgeHandle = NULL;
    Link = GetFirstNode (&RootBridgeList);
    while ((PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) &&
      (!IsNull (&RootBridgeList, Link))) {
      RootBridgeDev = PCI_IO_DEVICE_FROM_LINK (Link);
      //
      // Get the Bus information
      //
      Status = PciResAlloc->StartBusEnumeration (
                              PciResAlloc,
                              RootBridgeHandle,
                              (VOID **) &Configuration
                              );
      if (EFI_ERROR (Status)) {
        return Status;
      }

      //
      // Get the bus number to start with
      //
      StartBusNumber  = (UINT8) (Configuration->AddrRangeMin);

      ResetAllPpbBusNumber (
        RootBridgeDev,
        StartBusNumber
      );

      FreePool (Configuration);
      Link = GetNextNode (&RootBridgeList, Link);
      DestroyRootBridge (RootBridgeDev);
    }

    //
    // Wait for all HPC initialized
    //
    Status = AllRootHPCInitialized (STALL_1_SECOND * 15);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Notify the bus allocation phase is about to start for the 2nd time
    //
    NotifyPhase (PciResAlloc, EfiPciHostBridgeBeginBusAllocation);

    DEBUG((EFI_D_ERROR, "PCI Bus Second Scanning\n"));
    RootBridgeHandle = NULL;
    while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

      //
      // if a root bridge instance is found, create root bridge device for it
      //
      RootBridgeDev = CreateRootBridge (RootBridgeHandle);

      if (RootBridgeDev == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // Enumerate all the buses under this root bridge
      //
      Status = PciRootBridgeEnumerator (
                PciResAlloc,
                RootBridgeDev
                );

      DestroyRootBridge (RootBridgeDev);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    //
    // Notify the bus allocation phase is to end for the 2nd time
    //
    NotifyPhase (PciResAlloc, EfiPciHostBridgeEndBusAllocation);
  }

  //
  // Notify the resource allocation phase is to start
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeBeginResourceAllocation);

  RootBridgeHandle = NULL;
  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

    //
    // if a root bridge instance is found, create root bridge device for it
    //
    RootBridgeDev = CreateRootBridge (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = StartManagingRootBridge (RootBridgeDev);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    PciRootBridgeIo = RootBridgeDev->PciRootBridgeIo;
    Status          = PciRootBridgeIo->Configuration (PciRootBridgeIo, (VOID **) &Descriptors);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = PciGetBusRange (&Descriptors, &MinBus, NULL, NULL);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Determine root bridge attribute by calling interface of Pcihostbridge
    // protocol
    //
    DetermineRootBridgeAttributes (
      PciResAlloc,
      RootBridgeDev
      );

    //
    // Collect all the resource information under this root bridge
    // A database that records all the information about pci device subject to this
    // root bridge will then be created
    //
    Status = PciPciDeviceInfoCollector (
              RootBridgeDev,
              (UINT8) MinBus
              );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    InsertRootBridge (RootBridgeDev);

    //
    // Record the hostbridge handle
    //
    AddHostBridgeEnumerator (RootBridgeDev->PciRootBridgeIo->ParentHandle);
  }

  return EFI_SUCCESS;
}
