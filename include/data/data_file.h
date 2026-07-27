#pragma once

#include "../data.h"
#include "lilc/numbers.h"
#include <lilc/alloc.h>

// Should be at the beginning of every data file
// Byte Layout:
// CWD               | <version>           |
// ^                   ^
// Unique Identifier   Data format version
// (3 Bytes)           (8 Bytes)
typedef struct {
  u64 version;
} DataHeader;

#define DATA_HEADER_UID "CWD"

// header can be null if you do not care about the header
void data_file_read(const char *filename, DataHeader *header, DataMap *contents, Allocator *allocator);

void data_file_write(const char *filename, const DataHeader *header, const DataMap *contents, Allocator *allocator);
