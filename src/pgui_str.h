 
#ifndef __PGUI_STR_H 
#define __PGUI_STR_H

#ifdef __cplusplus
extern "C" {
#endif

int pgui_str_equal(const char* str1, const char* str2);
int pgui_str_equal_no_case(const char* str1, const char* str2);
int pgui_str_equal_no_case_no_space(const char* str1, const char* str2);
int pgui_str_equal_partial(const char* str1, const char* str2);
int pgui_str_equal_no_case_partial(const char* str1, const char* str2);
int pgui_str_boolean(const char* str);
int pgui_str_false(const char* str);
int pgui_str_line_count(const char* str);
const char* pgui_str_next_line(const char* str, int *len);
const char* pgui_str_next_value(const char* str, int str_len, int *len, char sep);
int pgui_str_count_char(const char *str, char c);
char* pgui_str_dup(const char* str); 
char *pgui_str_dup_until(const char **str, char c);
void pgui_str_copy_n(char* dst_str, int dst_max_size, const char* src_str);
char *pgui_str_get_memory(int size);
char *pgui_str_get_large_mem(int *size);
void pgui_str_lower(char* dstr, const char* sstr);
void pgui_str_upper(char* dstr, const char* sstr);
int pgui_str_has_space(const char* str);

#define pgui_isdigit(_c) (_c>='0' && _c<='9')
#define pgui_isnumber(_c) ((_c>='0' && _c<='9') || \
                          _c == '.'  || _c == ',' || \
                          _c == '-' || _c == '+' || \
                          _c == 'e' || _c == 'E')
#define pgui_toupper(_c)  ((_c >= 'a' && _c <= 'z')? (_c - 'a') + 'A': _c)
#define pgui_tolower(_c)  ((_c >= 'A' && _c <= 'Z')? (_c - 'A') + 'a': _c)

int pgui_str_is_ascii(const char* str);
char* pgui_str_return_strf(const char* format, ...);
char* pgui_str_return_str(const char* str);
char* pgui_str_return_boolean(int i);
char* pgui_str_return_checked(int i);
char* pgui_str_return_int(int i);
char* pgui_str_return_uint(unsigned int i);

#define PGUI_FLOAT2_STR "%.9f"

char* pgui_str_return_float(float f);
#define PGUI_DOUBLE2_STR "%.18f"

char* pgui_str_return_double(double d);
char* pgui_str_return_rgb(unsigned char r, unsigned char g, unsigned char b);
char* pgui_str_return_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
char* pgui_str_return_str_str(const char *str1, const char *str2, char sep);
char* pgui_str_return_int_int(int i1, int i2, char sep);
int pgui_str_get_format_precision(const char* format);
void pgui_str_printf_double_locale(char *str, const char *format, double d, const char* decimal_symbol);
int pgui_str_to_rgb(const char *str, unsigned char *r, unsigned char *g, unsigned char *b);
int pgui_str_to_rgbA(const char *str, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);
int pgui_str_to_int(const char *str, int *i);
int pgui_str_to_uint(const char *str, unsigned int *i);
int pgui_str_to_int_int(const char *str, int *i1, int *i2, char sep);
int pgui_str_to_float(const char *str, float *f);
int pgui_str_to_float_def(const char *str, float *f, float def);
int pgui_str_to_double(const char *str, double *d);
int pgui_str_to_double_def(const char *str, double *d, double def);
int pgui_str_to_double_locale(const char *str, double *d, const char* decimal_symbol);
int pgui_str_to_float_float(const char *str, float *f1, float *f2, char sep);
int pgui_str_to_double_double(const char *str, double *f1, double *f2, char sep);
int pgui_str_to_str_str(const char *str, char *str1, char *str2, char sep);
char* pgui_str_file_get_ext(const char *file_name);
char* pgui_str_file_get_title(const char *file_name);
char* pgui_str_file_get_path(const char *file_name);
char* pgui_str_file_make_file_name(const char* path, const char* title);
void pgui_str_file_name_split(const char* filename, char* path, char* title);
int pgui_str_replace(char* str, char src, char dst);
void pgui_str_to_unix(char* str);
void pgui_str_to_mac(char* str);
char* pgui_str_to_dos(const char* str);
char* pgui_str_convert_to_c(const char* str);
void pgui_str_remove(char* value, int start, int end, int dir, int utf8);
char* pgui_str_insert(const char* value, const char* insert_value, int start, int end, int utf8);
char* pgui_str_process_mnemonic(const char* str, char *c, int action);
int pgui_str_find_mnemonic(const char* str);
int pgui_str_compare(const char* str1, const char* str2, int casesensitive, int utf8);
int pgui_str_compare_equal(const char *str1, const char *str2, int casesensitive, int utf8, int partial);
int pgui_str_compare_find(const char *str1, const char *str2, int casesensitive, int utf8);

#ifdef __cplusplus
}
#endif

#endif
