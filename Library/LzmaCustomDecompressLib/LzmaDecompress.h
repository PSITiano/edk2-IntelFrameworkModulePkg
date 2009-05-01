/** @file
  LZMA Decompress Library header file

  Copyright (c) 2006 - 2009, Intel Corporation<BR>
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LZMADECOMPRESS_H__
#define __LZMADECOMPRESS_H__

/**
  The internal implementation of *_DECOMPRESS_PROTOCOL.GetInfo().
  
  @param Source           The source buffer containing the compressed data.
  @param SourceSize       The size of source buffer
  @param DestinationSize  The size of destination buffer.
  @param ScratchSize      The size of scratch buffer.

  @retval RETURN_SUCCESS           - The size of destination buffer and the size of scratch buffer are successull retrieved.
  @retval RETURN_INVALID_PARAMETER - The source data is corrupted
**/
RETURN_STATUS
EFIAPI
LzmaUefiDecompressGetInfo (
  IN  CONST VOID  *Source,
  IN  UINT32      SourceSize,
  OUT UINT32      *DestinationSize,
  OUT UINT32      *ScratchSize
  );

/**
  The internal implementation of *_DECOMPRESS_PROTOCOL.Decompress().
  
  @param Source          - The source buffer containing the compressed data.
  @param Destination     - The destination buffer to store the decompressed data
  @param Scratch         - The buffer used internally by the decompress routine. This  buffer is needed to store intermediate data.

  @retval RETURN_SUCCESS           - Decompression is successfull
  @retval RETURN_INVALID_PARAMETER - The source data is corrupted  
**/
RETURN_STATUS
EFIAPI
LzmaUefiDecompress (
  IN CONST VOID  *Source,
  IN OUT VOID    *Destination,
  IN OUT VOID    *Scratch
  );

#endif // __LZMADECOMPRESS_H__

