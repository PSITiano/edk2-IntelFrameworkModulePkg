/**@file

Copyright (c) 2006 - 2007, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __PCI_HOTPLUG_REQUEST_H_
#define __PCI_HOTPLUG_REQUEST_H_

#define EFI_PCI_HOTPLUG_REQUEST_PROTOCOL_GUID \
{0x19cb87ab,0x2cb9,{0x4665,0x83,0x60,0xdd,0xcf,0x60,0x54,0xf7,0x9d}}

typedef enum {
  EfiPciHotPlugRequestAdd,
  EfiPciHotplugRequestRemove
} EFI_PCI_HOTPLUG_OPERATION;

typedef struct _EFI_PCI_HOTPLUG_REQUEST_PROTOCOL  EFI_PCI_HOTPLUG_REQUEST_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_PCI_HOTPLUG_REQUEST_NOTIFY) (
 IN EFI_PCI_HOTPLUG_REQUEST_PROTOCOL *This,
 IN EFI_PCI_HOTPLUG_OPERATION        Operation,
 IN EFI_HANDLE                       Controller,
 IN EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath OPTIONAL,
 IN OUT UINT8                        *NumberOfChildren,
 IN OUT EFI_HANDLE                   *ChildHandleBuffer
);



struct _EFI_PCI_HOTPLUG_REQUEST_PROTOCOL {
  EFI_PCI_HOTPLUG_REQUEST_NOTIFY     Notify;
};

extern EFI_GUID gEfiPciHotPlugRequestProtocolGuid;

#endif
