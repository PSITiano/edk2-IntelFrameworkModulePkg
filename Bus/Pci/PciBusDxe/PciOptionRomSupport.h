/**@file

Copyright (c) 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef _EFI_PCI_OP_ROM_SUPPORT_H
#define _EFI_PCI_OP_ROM_SUPPORT_H

EFI_STATUS
GetOpRomInfo (
  IN PCI_IO_DEVICE    *PciIoDevice
  )
/**

Routine Description:

  TODO: Add function description

Arguments:

  PciIoDevice - TODO: add argument description

Returns:

  TODO: add return values

**/
;

EFI_STATUS
LoadOpRomImage (
  IN PCI_IO_DEVICE   *PciDevice,
  IN UINT64          RomBase
  )
/**

Routine Description:

  TODO: Add function description

Arguments:

  PciDevice - TODO: add argument description
  RomBase   - TODO: add argument description

Returns:

  TODO: add return values

**/
;

EFI_STATUS
RomDecode (
  IN PCI_IO_DEVICE   *PciDevice,
  IN UINT8           RomBarIndex,
  IN UINT32          RomBar,
  IN BOOLEAN         Enable
  )
/**

Routine Description:

  TODO: Add function description

Arguments:

  PciDevice   - TODO: add argument description
  RomBarIndex - TODO: add argument description
  RomBar      - TODO: add argument description
  Enable      - TODO: add argument description

Returns:

  TODO: add return values

**/
;

EFI_STATUS
ProcessOpRomImage (
  PCI_IO_DEVICE   *PciDevice
  )
/**

Routine Description:

  TODO: Add function description

Arguments:

  PciDevice - TODO: add argument description

Returns:

  TODO: add return values

**/
;

#endif
