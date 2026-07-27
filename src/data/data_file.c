#include "../../include/data/data_file.h"
#include "../../include/data/data_reader.h"
#include "../../include/bytebuf.h"
#include "lilc/log.h"
#include "../../include/shared.h"
#include <string.h>

static i32 byte_buf_read_header(ByteBuf *buf, DataHeader *header) {
  u8 uid[3];
  uid[0] = byte_buf_read_byte(buf);
  uid[1] = byte_buf_read_byte(buf);
  uid[2] = byte_buf_read_byte(buf);

  if (memcmp(uid, DATA_HEADER_UID, sizeof(uid)) != 0) {
    return 0;
  }

  u64 version = byte_buf_read_i64(buf);
  if (header != NULL) {
    header->version = version;
  }

  return 1;
}

static void byte_buf_write_header(ByteBuf *buf, const DataHeader *header) {
  byte_buf_write_byte(buf, DATA_HEADER_UID[0]);
  byte_buf_write_byte(buf, DATA_HEADER_UID[1]);
  byte_buf_write_byte(buf, DATA_HEADER_UID[2]);

  byte_buf_write_i64(buf, header->version);
}

void data_file_read(const char *filename, DataHeader *header, DataMap *contents, Allocator *allocator) {
  ByteBuf buf;
  byte_buf_init(&buf, allocator);

  byte_buf_from_file(&buf, filename, allocator);

  if (!byte_buf_read_header(&buf, header)) {
    log_error("Failed to read data header in file %s", filename);
    exit(1);
  }

  Data data = byte_buf_read_data(&buf);

  const char *string = data_reader_read_data(&data);

  FILE *f = fopen("world-data-aaa.json", "w");
  fprintf(f, "%s", string);
  fclose(f);

  if (data.type == DATA_TYPE_MAP) {
    *contents = data.var.data_map;
  } else {
    log_error("Contents of data file %s are not a map", filename);
    exit(1);
  }

  byte_buf_deinit(&buf);
}

void data_file_write(const char *filename, const DataHeader *header, const DataMap *contents, Allocator *allocator) {
  ByteBuf buf;
  byte_buf_init(&buf, allocator);

  byte_buf_write_header(&buf, header);

  Data map = data_map(*contents);
  byte_buf_write_data(&buf, &map);

  ensure_parent_dirs(filename, 0755);

  byte_buf_to_file(&buf, filename);

  byte_buf_deinit(&buf);
}