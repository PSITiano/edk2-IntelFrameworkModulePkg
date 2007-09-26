#/** @file
# Intel Framework Reference Module Package for All Architectures
#
# This file is used to build all modules in IntelFrameworkModulePkg.
#
# Copyright (c) 2007, Intel Corporation
#
# All rights reserved. This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution. The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#**/

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = IntelFrameworkModuleAll
  PLATFORM_GUID                  = FFF87D9A-E5BB-4AFF-9ADE-8645492E8087
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/IntelFrameworkModuleAll
  SUPPORTED_ARCHITECTURES        = IA32|IPF|X64|EBC
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

[LibraryClasses.common]
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  OemHookStatusCodeLib|IntelFrameworkModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  SerialPortLib|MdePkg/Library/SerialPortLibNull/SerialPortLibNull.inf
  S3Lib|IntelFrameworkModulePkg/Library/PeiS3Lib/PeiS3Lib.inf
  RecoveryLib|IntelFrameworkModulePkg/Library/PeiRecoveryLib/PeiRecoveryLib.inf
  
[LibraryClasses.common.PEIM]
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  IoLib|MdePkg/Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf

[LibraryClasses.common.DXE_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  DxeServiceTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  HiiLibFramework|IntelFrameworkPkg/Library/HiiLibFramework/HiiLib.inf
  IfrSupportLibFramework|IntelFrameworkPkg/Library/IfrSupportLibFramework/IfrSupportLib.inf
  PciIncompatibleDeviceSupportLib|IntelFrameworkModulePkg/Library/PciIncompatibleDeviceSupportLib/PciIncompatibleDeviceSupportLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  GraphicsLib|IntelFrameworkModulePkg/Library/GraphicsLib/GraphicsLib.inf
  
[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  DxeServiceTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  HiiLibFramework|IntelFrameworkPkg/Library/HiiLibFramework/HiiLib.inf
  IfrSupportLibFramework|IntelFrameworkPkg/Library/IfrSupportLibFramework/IfrSupportLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag.common]
  gEfiMdePkgTokenSpaceGuid.PcdComponentNameDisable|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnosticsDisable|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdComponentName2Disable|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnostics2Disable|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseOEM|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseEfiSerial|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseHardSerial|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseRuntimeMemory|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseDataHub|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeReplayInSerial|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeReplayInRuntimeMemory|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeReplayInDataHub|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeReplayInOEM|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPciIsaEnable|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPciVgaEnable|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPciBusHotplugDeviceSupport|TRUE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdIsaBusSupportDma|TRUE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdIsaBusOnlySupportSlaveDma|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdIsaBusSupportIsaMemory|TRUE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPciCfgDisable|TRUE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPciCfg2Disable|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdNtEmulatorEnable|FALSE

[PcdsFixedAtBuild.common]
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
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdPciIncompatibleDeviceSupportMask|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase|0xFFFB0000
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|0

  
[PcdsDynamicDefault.common.DEFAULT]
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeMemorySize|1
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeRuntimeMemorySize|4

[PcdsFixedAtBuild.IPF]
  gEfiMdePkgTokenSpaceGuid.PcdIoBlockBaseAddressForIpf|0x0ffffc000000

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################

[Components.common]
  IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  IntelFrameworkModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  IntelFrameworkModulePkg/Library/PciIncompatibleDeviceSupportLib/PciIncompatibleDeviceSupportLib.inf
  IntelFrameworkModulePkg/Library/GraphicsLib/GraphicsLib.inf

  IntelFrameworkModulePkg/Library/PeiS3Lib/PeiS3Lib.inf
  IntelFrameworkModulePkg/Library/PeiRecoveryLib/PeiRecoveryLib.inf
 
  IntelFrameworkModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  IntelFrameworkModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaBusDxe/IsaBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaFloppyDxe/IsaFloppyDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaSerialDxe/IsaSerialDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2keyboardDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf
  IntelFrameworkModulePkg/Universal/DataHubDxe/DataHubDxe.inf
  IntelFrameworkModulePkg/Universal/DataHubStdErrDxe/DataHubStdErrDxe.inf
  IntelFrameworkModulePkg/Universal/StatusCode/Pei/PeiStatusCode.inf
  IntelFrameworkModulePkg/Universal/HiiDataBaseDxe/HiiDatabase.inf
  IntelFrameworkModulePkg/Universal/SetupBrowserDxe/SetupBrowser.inf
  IntelFrameworkModulePkg/Universal/DriverSampleDxe/DriverSampleDxe.inf
  IntelFrameworkModulePkg/Universal/Console/VgaClassDxe/VgaClassDxe.inf
  IntelFrameworkModulePkg/Universal/PcatSingleSegmentPciCfgPei/PcatSingleSegmentPciCfgPei.inf
  IntelFrameworkModulePkg/Universal/VariablePei/VariablePei.inf
  IntelFrameworkModulePkg/Bus/Pci/VgaMiniPortDxe/VgaMiniPortDxe.inf

  IntelFrameworkModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  IntelFrameworkModulePkg/Library/BaseReportStatusCodeLib/BaseReportStatusCodeLib.inf
  IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf


[Components.IA32]
  IntelFrameworkModulePkg/Universal/StatusCode/Dxe/DxeStatusCode.inf

[Components.X64]
  IntelFrameworkModulePkg/Universal/StatusCode/Dxe/DxeStatusCode.inf
