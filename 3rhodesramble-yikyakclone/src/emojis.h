#ifndef __EMOJIS_H__
#define __EMOJIS_H__

#include <stdint.h>
#include <wchar.h>

// Null-terminated array of emojis.
extern const uint32_t emojis[];

// Returns the number of emojis.
int n_emoji();

// Returns emoji at index idx.
wchar_t emoji(int idx);

#endif
