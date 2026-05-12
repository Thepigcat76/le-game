#pragma once

#include "../bytebuf.h"
#include "../space.h"

void space_encode(const Space *space, ByteBuf *buf);

void space_decode(Space *space, ByteBuf *buf);