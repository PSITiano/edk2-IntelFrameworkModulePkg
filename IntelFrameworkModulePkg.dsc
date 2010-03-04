## @file
# Intel Framework Reference Module Package for All Architectures
#
# This file is used to build all modules in IntelFrameworkModulePkg.
#
# Copyright (c) 2007 - 2010, Intel Corporation
#
# All rights reserved. This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution. The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = IntelFrameworkModuleAll
  PLATFORM_GUID                  = FFF87D9A-E5BB-4AFF-9ADE-8645492E8087
  PLATFORM_VERSION               = 0.91
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/IntelFrameworkModuleAll
  SUPPORTED_ARCHITECTURES        = IA32|IPF|X64|EBC|ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this
#                              Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT              # The entry: 0|DEFAULT is reserved and always required.

[LibraryClasses]
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  S3Lib|IntelFrameworkModulePkg/Library/PeiS3Lib/PeiS3Lib.inf
  RecoveryLib|IntelFrameworkModulePkg/Library/PeiRecoveryLib/PeiRecoveryLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  GenericBdsLib|IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  PlatformBdsLib|IntelFrameworkModulePkg/Library/PlatformBdsLibNull/PlatformBdsLibNull.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf

[LibraryClasses.common.PEIM]
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  IoLib|MdePkg/Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

[LibraryClasses.common.DXE_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  IoLib|IntelFrameworkPkg/Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  IoLib|IntelFrameworkPkg/Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf

[LibraryClasses.common.UEFI_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  IoLib|IntelFrameworkPkg/Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  
################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
  gEfiMdePkgTokenSpaceGuid.PcdComponentNameDisable|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnosticsDisable|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdComponentName2Disable|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnostics2Disable|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdIsaBusSerialUseHalfHandshake|FALSE

[PcdsFixedAtBuild]
  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|10000000
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x0f
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x06
  gEfiMdePkgTokenSpaceGuid.PcdDebugClearMemoryValue|0xAF
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xE0000000
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|320
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|0

[PcdsFixedAtBuild.IPF]
  gEfiMdePkgTokenSpaceGuid.PcdIoBlockBaseAddressForIpf|0x0ffffc000000

###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[Components]
  IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  IntelFrameworkModulePkg/Library/PeiS3Lib/PeiS3Lib.inf
  IntelFrameworkModulePkg/Library/PeiRecoveryLib/PeiRecoveryLib.inf
  IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  IntelFrameworkModulePkg/Library/SmmRuntimeDxeReportStatusCodeLibFramework/SmmRuntimeDxeReportStatusCodeLibFramework.inf
  IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  IntelFrameworkModulePkg/Library/PlatformBdsLibNull/PlatformBdsLibNull.inf
  IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf

  IntelFrameworkModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaBusDxe/IsaBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaFloppyDxe/IsaFloppyDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaSerialDxe/IsaSerialDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2keyboardDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2MouseAbsolutePointerDxe/Ps2MouseAbsolutePointerDxe.inf
  IntelFrameworkModulePkg/Bus/Pci/VgaMiniPortDxe/VgaMiniPortDxe.inf

  IntelFrameworkModulePkg/Universal/SectionExtractionDxe/SectionExtractionDxe.inf
  IntelFrameworkModulePkg/Universal/DataHubDxe/DataHubDxe.inf
  IntelFrameworkModulePkg/Universal/DataHubStdErrDxe/DataHubStdErrDxe.inf
  IntelFrameworkModulePkg/Universal/StatusCode/Pei/StatusCodePei.inf
  IntelFrameworkModulePkg/Universal/Console/VgaClassDxe/VgaClassDxe.inf
  IntelFrameworkModulePkg/Universal/BdsDxe/BdsDxe.inf
  IntelFrameworkModulePkg/Universal/LegacyRegionDxe/LegacyRegionDxe.inf

[Components.IA32]
  IntelFrameworkModulePkg/Universal/StatusCode/RuntimeDxe/StatusCodeRuntimeDxe.inf
  IntelFrameworkModulePkg/Universal/CpuIoDxe/CpuIoDxe.inf {
    <LibraryClasses>
      IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  }

[Components.X64]
  IntelFrameworkModulePkg/Universal/StatusCode/RuntimeDxe/StatusCodeRuntimeDxe.inf
  IntelFrameworkModulePkg/Universal/CpuIoDxe/CpuIoDxe.inf {
    <LibraryClasses>
      IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  }
  
[Components.IPF]
  IntelFrameworkModulePkg/Universal/CpuIoDxe/CpuIoDxe.inf {
    <LibraryClasses>
      IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  }
