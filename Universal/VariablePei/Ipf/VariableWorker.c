/** @file
  Variable worker functions.

  Copyright (c) 2006, Intel Corporation<BR>
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Variable.h>


/**
  Get one variable by the index count.

  @param  IndexTable  The pointer to variable index table.
  @param  Count       The index count of variable in index table.

  @return The pointer to variable header indexed by count.

**/
VARIABLE_HEADER *
GetVariableByIndex (
  IN VARIABLE_INDEX_TABLE        *IndexTable,
  IN UINT32                      Count
  )
{
  return (VARIABLE_HEADER *) (UINTN) ((((UINT32)IndexTable->Index[Count]) << 2) + ((UINT32)(UINTN)IndexTable->StartPtr & 0xFFFC0000) );
}

/**
  Record Variable in VariableIndex HOB.

  Record Variable in VariableIndex HOB and update the length of variable index table.

  @param  IndexTable  The pointer to variable index table.
  @param  Variable    The pointer to the variable that will be recorded.

  @retval VOID

**/
VOID
VariableIndexTableUpdate (
  IN OUT  VARIABLE_INDEX_TABLE   *IndexTable,
  IN      VARIABLE_HEADER        *Variable
  )
{
  IndexTable->Index[IndexTable->Length++] = (UINT16) (((UINT32)(UINTN) Variable) >> 2);

  return;
}

