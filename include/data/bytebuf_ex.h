#pragma once

#include "../bytebuf.h"
#include "../space.h"
#include "data_ex.h"

void space_encode(const Space *space, ByteBuf *buf, DataContext ctx);

void space_decode(Space *space, ByteBuf *buf, DataContext ctx);