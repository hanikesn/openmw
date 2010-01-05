/*
  OpenMW - The completely unofficial reimplementation of Morrowind
  Copyright (C) 2008-2010  Nicolay Korslund
  Email: < korslund@gmail.com >
  WWW: http://openmw.sourceforge.net/

  This file (nif_file.h) is part of the OpenMW package.

  OpenMW is distributed as free software: you can redistribute it
  and/or modify it under the terms of the GNU General Public License
  version 3, as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  version 3 along with this program. If not, see
  http://www.gnu.org/licenses/ .

 */

#ifndef _NIF_FILE_H_
#define _NIF_FILE_H_

#include "../mangle/stream/stream.h"
#include "../mangle/stream/filters/buffer_stream.h"
#include "../mangle/tools/str_exception.h"

#include "../tools/slice_array.h"

#include <vector>
#include <string>

#include "record.h"

using namespace Mangle::Stream;

class NIFFile
{
  enum NIFVersion
    {
      VER_MW    = 0x04000002    // Morrowind NIFs
    };

  /// Nif file version
  int ver;

  /// Input stream
  StreamPtr inp;

  /// File name, used for error messages
  std::string filename;

  /// Record list
  std::vector<NifRecord*> records;

  /// Used for error handling
  void fail(const std::string &msg)
    {
      std::string err = "NIFFile Error: " + msg;
      err += "\nFile: " + filename;
      throw str_exception(err);
    }

  /// Parse the file
  void parse();

 public:
  /// Open a NIF stream. The name is used for error messages.
  NIFFile(StreamPtr nif, const std::string &name)
    : filename(name)
    {
      /* Load the entire file into memory. This allows us to use
         direct pointers to the data arrays in the NIF, instead of
         individually allocating and copying each one.

         The NIF data is only stored temporarily in memory, since once
         the mesh data is loaded it is siphoned into OGRE and
         deleted. For that reason, we might improve this further and
         use a shared region/pool based allocation scheme in the
         future, especially since only one NIFFile will ever be loaded
         at any given time.
      */
      inp = StreamPtr(new BufferStream(nif));

      parse();
    }


  /* ************************************************

               Parser functions

  ****************************************************/

  template<class X> X getType() { return *((X*)inp->getPtr(sizeof(X))); }
  int getInt() { return getType<int>(); }

  template<class X>
  SliceArray<X> getArray()
    {
      int len = getInt();
      return SliceArray<X>((const X*)inp->getPtr(len), len);
    }

  SString getString() { return getArray<char>(); }

  const char *getString(int size)
    { return (const char*)inp->getPtr(size); }
};
#endif