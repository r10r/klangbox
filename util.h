#include <string.h>
#include <stddef.h>     /* NULL */
#include <stdlib.h>     /* calloc */

#define cx_alloc(n) calloc(1, n)
#define cx_free(n) free(n)

unsigned
str_cnt(const char* s, const char* sep);

unsigned
str_split(char* s, const char* sep, const char*** arr);

unsigned
str_arr_intersect(const char** a, unsigned num_a, const char** b, unsigned num_b, const char*** c);

unsigned
str_arr_diff(const char** a, unsigned num_a, const char** b, const char*** c);

char*
str_arr_join(const char** a, const char* sep);

inline int
str_arr_len(const char** a);
