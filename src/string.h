#pragma once

#ifndef STRING_H
#define STRING_H

//general extensions
int block_checksum(char* line);
void str_append(char* to_append, const char* append_with);
unsigned long file_path_hash(char* path);
unsigned long dj2b(char* identifier);

#endif // !FLIB_H
