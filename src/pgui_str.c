#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <limits.h>
#include <stdarg.h>
#include <locale.h>
#include "pgui_str.h"

/* Line breaks can be:
   \n - UNIX
   \r - Mac
   \r\n - DOS/Windows
   */

#define PGUI_STR_EQUAL(str1, str2)      \
{                                      \
	if (str1 == str2)                    \
	return 1;                          \
	\
	if (!str1 || !str2)                  \
	return 0;                          \
	\
	while(*str1 && *str2 &&              \
			SF(*str1) == SF(*str2))        \
	{                                    \
		EXTRAINC(str1);                    \
		EXTRAINC(str2);                    \
		str1++;                            \
		str2++;                            \
	}                                    \
	\
	/* check also for terminator */      \
	if (*str1 == *str2) return 1;        \
}


int 
pgui_str_equal(const char* str1, const char* str2) {
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) (_x)
	PGUI_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
	return 0;
}


int 
pgui_str_equal_partial(const char* str1, const char* str2) {
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) (_x)
	PGUI_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
	if (*str2 == 0) 
		return 1;  /* if second string is at terminator, then it is partially equal */
	return 0;
}

int 
pgui_str_equal_no_case(const char* str1, const char* str2) {
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) pgui_tolower(_x)
	PGUI_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
	return 0;
}

int 
pgui_str_equal_no_case_partial(const char* str1, const char* str2) {
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) pgui_tolower(_x)
	PGUI_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
	if (*str2 == 0) 
		return 1;  /* if second string is at terminator, then it is partially equal */
	return 0;
}

int 
pgui_str_equal_no_case_no_space(const char* str1, const char* str2) {
#define EXTRAINC(_x) { if (*_x == ' ') _x++; }  /* also ignore spaces */
#define SF(_x) pgui_tolower(_x)
	PGUI_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
	return 0;
}

int 
pgui_str_false(const char* str) {
	if (!str || str[0]==0) return 0;
	if (str[0]=='0' && str[1]==0) return 1;
	if (pgui_str_equal_no_case(str, "NO")) return 1;
	if (pgui_str_equal_no_case(str, "OFF")) return 1;
	return 0;
}

int 
pgui_str_boolean(const char* str) {
	if (!str || str[0]==0) return 0;
	if (str[0]=='1' && str[1]==0) return 1;
	if (pgui_str_equal_no_case(str, "YES")) return 1;
	if (pgui_str_equal_no_case(str, "ON")) return 1;
	return 0;
}

void 
pgui_str_upper(char* dstr, const char* sstr) {
	if (!sstr || sstr[0] == 0) return;
	for (; *sstr; sstr++, dstr++)
		*dstr = (char)pgui_toupper(*sstr);
	*dstr = 0;
}

void 
pgui_str_lower(char* dstr, const char* sstr) {
	if (!sstr || sstr[0] == 0) return;
	for (; *sstr; sstr++, dstr++)
		*dstr = (char)pgui_tolower(*sstr);
	*dstr = 0;
}

int 
pgui_str_has_space(const char* str) {
	if (!str) return 0;
	while (*str) {
		if (*str == ' ')
			return 1;
		str++;
	}
	return 0;
}

char *
pgui_str_dup(const char *str) {
	if (str) {
		int size = (int)strlen(str)+1;
		char *newstr = malloc(size);
		if (newstr) memcpy(newstr, str, size);
		return newstr;
	}
	return NULL;
}

const char * 
pgui_str_next_line(const char* str, int *len) {
	*len = 0;
	if (!str) return NULL;
	while(*str!=0 && *str!='\n' && *str!='\r') {
		(*len)++;
		str++;
	}
	if (*str=='\r' && *(str+1)=='\n')   
		return str+2;
	else if (*str=='\n' || *str=='\r')   
		return str+1;
	else 
		return str;  
}

const char * 
pgui_str_next_value(const char* str, int str_len, int *len, char sep) {
	int ignore_sep = 0;
	*len = 0;
	if (!str) return NULL;
	while (*str != 0 && (*str != sep || ignore_sep) && *len<str_len) {
		if (*str == '\"') {
			if (ignore_sep)
				ignore_sep = 0;
			else
				ignore_sep = 1;
		}
		(*len)++;
		str++;
	}
	if (*str==sep)
		return str+1;
	else 
		return str;  
}

int 
pgui_str_line_count(const char* str) {
	int num_lin = 1;
	if (!str)
		return num_lin;
	while(*str != 0) {
		while(*str!=0 && *str!='\n' && *str!='\r')
			str++;
		if (*str=='\r' && *(str+1)=='\n')   {
			num_lin++;
			str+=2;
		}
		else if (*str=='\n' || *str=='\r')  {
			num_lin++;
			str++;
		}
	}
	return num_lin;
}

int 
pgui_str_count_char(const char *str, char c) {
	int n;
	if (!str) return 0;
	for (n=0; *str; str++) {
		if (*str==c)
			n++;
	}
	return n;
}

void 
pgui_str_copy_n(char* dst_str, int dst_max_size, const char* src_str) {
	if (src_str) {
		int size = (int)strlen(src_str) + 1;
		if (size > dst_max_size) size = dst_max_size;
		memcpy(dst_str, src_str, size - 1);
		dst_str[size - 1] = 0;
	}
}

char * 
pgui_str_dup_until(const char **str, char c) {
	const char *p_str;
	char *new_str;
	if (!str || str[0]==0)
		return NULL;
	p_str = strchr(*str,c);
	if (!p_str) 
		return NULL;
	else {
		int i;
		int sl = (int)(p_str - (*str));
		new_str = (char *)malloc(sl + 1);
		if (!new_str) return NULL;
		for (i = 0; i < sl; ++i)
			new_str[i] = (*str)[i];
		new_str[sl] = 0;
		*str = p_str+1;
		return new_str;
	}
}

static char * 
i_str_dup_until_no_case(char **str, char sep) {
	char *p_str,*new_str;
	if (!str || str[0]==0)
		return NULL;
	p_str=strchr(*str,sep); 
	if (!p_str && (pgui_toupper(sep)!=sep)) 
		p_str=strchr(*str, pgui_toupper(sep));  
	
	if (!p_str) 
		return NULL;
	else {
		int i;
		int sl=(int)(p_str - (*str));
		new_str = (char *) malloc (sl + 1);
		if (!new_str) return NULL;
		for (i = 0; i < sl; ++i)
			new_str[i] = (*str)[i];
		new_str[sl] = 0;
		*str = p_str+1;
		return new_str;
	}
}

char *
pgui_str_get_large_mem(int *size) {
#define LARGE_MAX_BUFFERS 10
#define LARGE_SIZE SHRT_MAX
	static char buffers[LARGE_MAX_BUFFERS][LARGE_SIZE];
	static int buffers_index = -1;
	char* ret_str;
	
	if (buffers_index == -1) {
		int i;
		
		for (i=0; i<LARGE_MAX_BUFFERS; i++)
			memset(buffers[i], 0, sizeof(char)*LARGE_SIZE);
		buffers_index = 0;
	}
	
	ret_str = buffers[buffers_index];
	ret_str[0] = 0;
	buffers_index++;
	if (buffers_index == LARGE_MAX_BUFFERS)
		buffers_index = 0;
	if (size) *size = LARGE_SIZE;
	return ret_str;
#undef LARGE_MAX_BUFFERS
#undef LARGE_SIZE 
}

static char * 
pgui_str_get_small_mem(void) {
#define SMALL_MAX_BUFFERS 100
#define SMALL_SIZE 80  
	static char buffers[SMALL_MAX_BUFFERS][SMALL_SIZE];
	static int buffers_index = -1;
	char* ret_str;
	
	if (buffers_index == -1) {
		int i;
		
		for (i = 0; i<SMALL_MAX_BUFFERS; i++)
			memset(buffers[i], 0, sizeof(char)*SMALL_SIZE);
		buffers_index = 0;
	}
	
	memset(buffers[buffers_index], 0, SMALL_SIZE);
	ret_str = buffers[buffers_index];
	buffers_index++;
	if (buffers_index == SMALL_MAX_BUFFERS)
		buffers_index = 0;
	return ret_str;
#undef SMALL_MAX_BUFFERS
#undef SMALL_SIZE 
}

char *
pgui_str_get_memory(int size) {
#define MAX_BUFFERS 50
	static char* buffers[MAX_BUFFERS];
	static int buffers_sizes[MAX_BUFFERS];
	static int buffers_index = -1;
	int i;
	if (size == -1) {
		buffers_index = -1;
		for (i = 0; i < MAX_BUFFERS; i++) {
			if (buffers[i]) {
				free(buffers[i]);
				buffers[i] = NULL;
			}
			buffers_sizes[i] = 0;
		}
		return NULL;
	}
	else {
		char* ret_str;
		
		if (buffers_index == -1) {
			memset(buffers, 0, sizeof(char*)*MAX_BUFFERS);
			memset(buffers_sizes, 0, sizeof(int)*MAX_BUFFERS);
			buffers_index = 0;
		}
		
		if (!(buffers[buffers_index])) {
			buffers_sizes[buffers_index] = size+1;
			buffers[buffers_index] = (char*)malloc(buffers_sizes[buffers_index]);
		}
		else if (buffers_sizes[buffers_index] < size+1)  {
			buffers_sizes[buffers_index] = size+1;
			buffers[buffers_index] = (char*)realloc(buffers[buffers_index], buffers_sizes[buffers_index]);
		}
		
		memset(buffers[buffers_index], 0, buffers_sizes[buffers_index]);
		ret_str = buffers[buffers_index];
		buffers_index++;
		if (buffers_index == MAX_BUFFERS)
			buffers_index = 0;
		return ret_str;
	}
#undef MAX_BUFFERS
}

char * 
pgui_str_return_strf(const char* format, ...) {
	char* str = pgui_str_get_memory(1024);
	va_list arglist;
	va_start(arglist, format);
	vsnprintf(str, 1024, format, arglist);
	va_end(arglist);
	return str;
}

char * 
pgui_str_return_str(const char* str) {
	if (str) {
		int size = (int)strlen(str)+1;
		char* ret_str = pgui_str_get_memory(size);
		memcpy(ret_str, str, size);
		return ret_str;
	}
	else
		return NULL;
}

char * 
pgui_str_return_boolean(int b) {
	if (b)
		return "YES";
	else
		return "NO";
}

char * 
pgui_str_return_checked(int check) {
	if (check == -1)
		return "NOTDEF";
	else if (check)
		return "ON";
	else
		return "OFF";
}

char * 
pgui_str_return_uInt(unsigned int i) {
	char* str = pgui_str_get_small_mem();  
	sprintf(str, "%u", i);
	return str;
}

char * 
pgui_str_return_int(int i) {
	char* str = pgui_str_get_small_mem();  
	sprintf(str, "%d", i);
	return str;
}

char * 
pgui_str_return_float(float f) {
	char* str = pgui_str_get_small_mem();  
	sprintf(str, PGUI_FLOAT2_STR, f);
	return str;
}

char * 
pgui_str_return_double(double d) {
	char* str = pgui_str_get_small_mem();  
	sprintf(str, PGUI_DOUBLE2_STR, d);
	return str;
}

char *
pgui_str_return_rGB(unsigned char r, unsigned char g, unsigned char b) {
	char* str = pgui_str_get_small_mem();  
	sprintf(str, "%d %d %d", (int)r, (int)g, (int)b);
	return str;
}

char * 
pgui_str_return_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	char* str = pgui_str_get_small_mem();  
	sprintf(str, "%d %d %d %d", (int)r, (int)g, (int)b, (int)a);
	return str;
}

char * 
pgui_str_return_str_str(const char *str1, const char *str2, char sep) {
	if (str1 || str2) {
		char* ret_str;
		int size1=0, size2=0;
		if (str1) size1 = (int)strlen(str1);
		if (str2) size2 = (int)strlen(str2);
		ret_str = pgui_str_get_memory(size1+size2+2);
		if (str1 && size1) memcpy(ret_str, str1, size1);
		ret_str[size1] = sep;
		if (str2 && size2) memcpy(ret_str+size1+1, str2, size2);
		ret_str[size1+1+size2] = 0;
		return ret_str;
	}
	else
		return NULL;
}

char * 
pgui_str_return_int_int(int i1, int i2, char sep) {
	char* str = pgui_str_get_small_mem();  
	sprintf(str, "%d%c%d", i1, sep, i2);
	return str;
}

int 
pgui_str_get_format_precision(const char* format) {
	int precision;
	while (*format) {
		if (*format == '.')
			break;
		format++;
	}
	if (*format != '.')
		return -1;
	format++;
	if (pgui_str_to_int(format, &precision))
		return precision;
	return -1;
}

int 
pgui_str_to_rGB(const char *str, unsigned char *r, unsigned char *g, unsigned char *b) {
	unsigned int ri = 0, gi = 0, bi = 0;
	if (!str) return 0;
	if (str[0]=='#') {
		str++;
		if (sscanf(str, "%2X%2X%2X", &ri, &gi, &bi) != 3) return 0;
	}
	else {
		if (sscanf(str, "%u %u %u", &ri, &gi, &bi) != 3) return 0;
	}
	if (ri > 255 || gi > 255 || bi > 255) return 0;
	*r = (unsigned char)ri;
	*g = (unsigned char)gi;
	*b = (unsigned char)bi;
	return 1;
}

int 
pgui_str_to_rgba(const char *str, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
	unsigned int ri = 0, gi = 0, bi = 0, ai = 255;
	if (!str) return 0;
	if (str[0] == '#') {
		str++;
		if (sscanf(str, "%2X%2X%2X%2X", &ri, &gi, &bi, &ai) < 3) return 0;
	}
	else {
		if (sscanf(str, "%u %u %u %u", &ri, &gi, &bi, &ai) < 3) return 0;
	}
	if (ri > 255 || gi > 255 || bi > 255 || ai > 255) return 0;
	*r = (unsigned char)ri;
	*g = (unsigned char)gi;
	*b = (unsigned char)bi;
	*a = (unsigned char)ai;
	return 1;
}


int 
pgui_str_to_int(const char *str, int *i) {
	if (!str) return 0;
	if (sscanf(str, "%d", i) != 1) return 0;
	return 1;
}

int 
pgui_str_to_uInt(const char *str, unsigned int *i) {
	if (!str) return 0;
	if (sscanf(str, "%u", i) != 1) return 0;
	return 1;
}

int 
pgui_str_to_int_int(const char *str, int *i1, int *i2, char sep) {
	if (!str) return 0;
	if (pgui_tolower(*str) == sep) {
		str++; 
		if (sscanf(str, "%d", i2) != 1) return 0;
		return 1;
	}
	else {
		char* p_str = i_str_dup_until_no_case((char**)&str, sep);
		if (!p_str)   {        
			if (sscanf(str, "%d", i1) != 1) return 0;
			return 1;
		}
		else if (*str==0)  {        
			int ret = sscanf(p_str, "%d", i1);
			free(p_str);
			if (ret != 1) return 0;
			return 1;
		}
		else {
			int ret = 0;
			if (sscanf(p_str, "%d", i1) == 1) ret++;
			if (sscanf(str, "%d", i2) == 1) ret++;
			free(p_str);
			return ret;
		}
	}
}

int 
pgui_str_to_float_def(const char *str, float *f, float def) {
	if (!str) { *f = def;  return 1; }
	if (sscanf(str, "%f", f) != 1) return 0;
	return 1;
}

int 
pgui_str_to_float(const char *str, float *f) {
	if (!str) return 0;
	if (sscanf(str, "%f", f) != 1) return 0;
	return 1;
}

int 
pgui_str_to_double(const char *str, double *d) {
	if (!str) return 0;
	if (sscanf(str, "%lf", d) != 1) return 0;
	return 1;
}

int 
pgui_str_to_double_def(const char *str, double *d, double def) {
	if (!str) { *d = def;  return 1; }
	if (sscanf(str, "%lf", d) != 1) return 0;
	return 1;
}

int 
pgui_str_to_float_float(const char *str, float *f1, float *f2, char sep) {
	if (!str) return 0;
	if (pgui_tolower(*str) == sep) {
		str++; 
		if (sscanf(str, "%f", f2) != 1) return 0;
		return 1;
	}
	else {
		char* p_str = i_str_dup_until_no_case((char**)&str, sep);
		if (!p_str)   {        
			if (sscanf(str, "%f", f1) != 1) return 0;
			return 1;
		}
		else if (*str==0)    {        
			int ret = sscanf(p_str, "%f", f1);
			free(p_str);
			if (ret != 1) return 0;
			return 1;
		}
		else {
			int ret = 0;
			if (sscanf(p_str, "%f", f1) == 1) ret++;
			if (sscanf(str, "%f", f2) == 1) ret++;
			free(p_str);
			return ret;
		}
	}
}

int 
pgui_str_to_double_double(const char *str, double *f1, double *f2, char sep) {
	if (!str) return 0;
	if (pgui_tolower(*str) == sep) {
		str++; 
		if (sscanf(str, "%lf", f2) != 1) return 0;
		return 1;
	}
	else {
		char* p_str = i_str_dup_until_no_case((char**)&str, sep);
		if (!p_str)   {
			if (sscanf(str, "%lf", f1) != 1) return 0;
			return 1;
		}
		else if (*str == 0)    {
			int ret = sscanf(p_str, "%lf", f1);
			free(p_str);
			if (ret != 1) return 0;
			return 1;
		}
		else {
			int ret = 0;
			if (sscanf(p_str, "%lf", f1) == 1) ret++;
			if (sscanf(str, "%lf", f2) == 1) ret++;
			free(p_str);
			return ret;
		}
	}
}

int 
pgui_str_to_str_str(const char *str, char *str1, char *str2, char sep) {
	str1[0] = 0;
	str2[0] = 0;
	if (!str)
		return 0;
	if (pgui_tolower(*str) == sep) {
		str++; 
		strcpy(str2, str);
		return 1;
	}
	else {
		char* p_str = i_str_dup_until_no_case((char**)&str, sep);
		if (!p_str)   {        
			strcpy(str1, str);
			return 1;
		}
		else if (*str==0)    {        
			strcpy(str1, p_str);
			free(p_str);
			return 1;
		}
		else {
			strcpy(str1, p_str);
			strcpy(str2, str);
			free(p_str);
			return 2;
		}
	}
}

char * 
pgui_str_file_get_path(const char *file_name) {
	if (!file_name)
		return NULL;
	else {
		
		int len = (int)strlen(file_name) - 1;
		while (len != 0) {
			if (file_name[len] == '\\' || file_name[len] == '/') {
				len++;
				break;
			}
			len--;
		}
		if (len == 0)
			return NULL; {
			char* path = malloc(len + 1);
			memcpy(path, file_name, len);
			path[len] = 0;
			return path;
		}
	}
}

char * 
pgui_str_file_get_title(const char *file_name) {
	if (!file_name)
		return NULL;
	else {
		
		int len = (int)strlen(file_name);
		int offset = len - 1;
		while (offset != 0) {
			if (file_name[offset] == '\\' || file_name[offset] == '/') {
				offset++;
				break;
			}
			offset--;
		}
		{
			int title_size = len - offset + 1;
			char* file_title = malloc(title_size);
			memcpy(file_title, file_name + offset, title_size);
			return file_title;
		}
	}
}

char * 
pgui_str_file_get_ext(const char *file_name) {
	if (!file_name)
		return NULL;
	else {
		
		int len = (int)strlen(file_name);
		int offset = len - 1;
		while (offset != 0) {
			
			if (file_name[offset] == '\\' || file_name[offset] == '/')
				return NULL;
			if (file_name[offset] == '.') {
				offset++;
				break;
			}
			offset--;
		}
		if (offset == 0)
			return NULL; 
		{
			int ext_size = len - offset + 1;
			char* file_ext = (char*)malloc(ext_size);
			memcpy(file_ext, file_name + offset, ext_size);
			return file_ext;
		}
	}
}

char * 
pgui_str_file_make_file_name(const char* path, const char* title) {
	if (!path || !title)
		return NULL;
	else {
		int size_path = (int)strlen(path);
		int size_title = (int)strlen(title);
		char *file_name = malloc(size_path + size_title + 2);
		memcpy(file_name, path, size_path);
		if (path[size_path - 1] != '/') {
			file_name[size_path] = '/';
			size_path++;
		}
		memcpy(file_name + size_path, title, size_title);
		file_name[size_path + size_title] = 0;
		return file_name;
	}
}

void 
pgui_str_file_name_split(const char* file_name, char *path, char *title) {
	int i, n;
	if (!file_name)
		return;
	
	n = (int)strlen(file_name);
	for (i = n - 1; i >= 0; i--) {
		if (file_name[i] == '\\' || file_name[i] == '/') {
			if (path) {
				strncpy(path, file_name, i+1);
				path[i+1] = 0;
			}
			if (title) {
				strcpy(title, file_name+i+1);
				title[n-i] = 0;
			}
			return;
		}
	}
}

int 
pgui_str_replace(char* str, char src, char dst) {
	int i = 0;
	if (!str)
		return 0;
	while (*str) {
		if (*str == src) {
			*str = dst;
			i++;
		}
		str++;
	}
	return i;
}

void 
pgui_str_to_unix(char* str) {
	char* pstr = str;
	if (!str) return;
	while (*str) {
		if (*str == '\r') {
			if (*(str+1) != '\n')  
				*pstr++ = '\n';
			str++;
		}
		else
			*pstr++ = *str++;
	}
	*pstr = *str;
}

void 
pgui_str_to_mac(char* str) {
	char* pstr = str;
	if (!str) return;
	while (*str) {
		if (*str == '\r') {
			if (*(++str) == '\n')  
				str++;
			*pstr++ = '\r';
		}
		else if (*str == '\n')  {
			str++;
			*pstr++ = '\r';
		}
		else
			*pstr++ = *str++;
	}
	*pstr = *str;
}

char * 
pgui_str_to_dos(const char* str) {
	char *auxstr, *newstr;
	int num_lin;
	if (!str) return NULL;
	num_lin = pgui_str_line_count(str);
	if (num_lin == 1)
		return (char*)str;
	newstr = malloc(num_lin + strlen(str) + 1);
	auxstr = newstr;
	while(*str) {
		if (*str == '\r' && *(str+1)=='\n')  {
			*auxstr++ = *str++;
			*auxstr++ = *str++;
		}
		else if (*str == '\r')   {
			*auxstr++ = *str++;
			*auxstr++ = '\n';
		}
		else if (*str == '\n')  {
			*auxstr++ = '\r';
			*auxstr++ = *str++;
		}
		else
			*auxstr++ = *str++;
	}
	*auxstr = 0;
	return newstr;	
}

#define PGUI_ISRESERVED(_c) (_c=='\n' || _c=='\r' || _c=='\t')
char * 
pgui_str_convert_to_c(const char* str) {
	char* new_str, *pnstr;
	const char* pstr = str;
	int len, count=0;
	if (!str)
		return NULL;
	while(*pstr) {
		if (PGUI_ISRESERVED(*pstr))
			count++;
		pstr++;
	}
	if (!count)
		return (char*)str;
	len = (int)(pstr-str);
	new_str = malloc(len+count+1);
	pstr = str;
	pnstr = new_str;
	while(*pstr) {
		if (PGUI_ISRESERVED(*pstr)) {
			*pnstr = '\\';
			pnstr++;
			switch(*pstr) {
				case '\n':
					*pnstr = 'n';
					break;
				case '\r':
					*pnstr = 'r';
					break;
				case '\t':
					*pnstr = 't';
					break;
			}
		}
		else
			*pnstr = *pstr;
		pnstr++;
		pstr++;
	}
	*pnstr = 0;
	return new_str;
}

char * 
pgui_str_process_mnemonic(const char* str, char *c, int action) {
	int i = 0, found = 0;
	char* new_str, *orig_str = (char*)str;
	if (!str) 
		return NULL;
	if (!strchr(str, '&'))
		return (char*)str;
	new_str = malloc(strlen(str)+1);
	while (*str) {
		if (*str == '&') {
			if (*(str+1) == '&') {
				found = -1;
				str++;
				new_str[i++] = *str;
			}
			else if (found!=1) {
				found = 1;
				if (action == 1) 
					new_str[i++] = *c;
				else if (action == -1)  
					*c = *(str+1);  
				
			}
		}
		else {
			new_str[i++] = *str;
		}
		str++;
	}
	new_str[i] = 0;
	if (found==0) {
		free(new_str);
		return orig_str;
	}
	return new_str;
}

int 
pgui_str_find_mnemonic(const char* str) {
	int c = 0, found = 0;
	if (!str) 
		return 0;
	if (!strchr(str, '&'))
		return 0;
	while (*str) {
		if (*str == '&') {
			if (*(str+1) == '&') {
				found = -1;
				str++;
			}
			else if (found!=1) {
				found = 1;
				c = *(str+1);  
			}
		}
		str++;
	}
	if (found==0)
		return 0;
	else
		return c;
}

static unsigned char* Latin1_map = NULL;
static unsigned char* Latin1_map_nocase = NULL;
static void i_str_init_latin1_map(void) {
	static unsigned char map[256];
	static unsigned char map_nocase[256];
	Latin1_map = map;
	Latin1_map_nocase = map_nocase;
#define mm(_x) (map[(unsigned char)_x])
	
	mm(  0)=  0;  mm(  1)=  1; mm(  2)=  2; mm(  3)=  3; mm(  4)=  4; mm(  5)=  5; mm(  6)=  6; mm(  7)=  7;  mm(  8)=  8; mm(  9)=  9; mm( 10)= 10; mm( 11)= 11; mm( 12)= 12; mm( 13)= 13; mm( 14)= 14; mm( 15)= 15; 
	mm( 16)= 16;  mm( 17)= 17; mm( 18)= 18; mm( 19)= 19; mm( 20)= 20; mm( 21)= 21; mm( 22)= 22; mm( 23)= 23;  mm( 24)= 24; mm( 25)= 25; mm( 26)= 26; mm( 27)= 27; mm( 28)= 28; mm( 29)= 29; mm( 30)= 30; mm( 31)= 31; 
	mm('\'')= 32; mm('-')= 33; mm('–')= 34; mm('—')= 35; mm(' ')= 36; mm('!')= 37; mm('"')= 38; mm('#')= 39;  mm('$')= 40; mm('%')= 41; mm('&')= 42; mm('(')= 43; mm(')')= 44; mm('*')= 45; mm(',')= 46; mm('.')= 47; 
	mm('/')= 48;  mm(':')= 49; mm(';')= 50; mm('?')= 51; mm('@')= 52; mm('(')= 53; mm(')')= 54; mm('^')= 55;  mm('ˆ')= 56; mm('_')= 57; mm('`')= 58; mm('{')= 59; mm('|')= 60; mm('}')= 61; mm('~')= 62; mm('¡')= 63; 
	mm('¦')= 64;  mm('¨')= 65; mm('¯')= 66; mm('´')= 67; mm('¸')= 68; mm('¿')= 69; mm('˜')= 70; mm('‘')= 71;  mm('’')= 72; mm('‚')= 73; mm('“')= 74; mm('”')= 75; mm('„')= 76; mm('‹')= 77; mm('›')= 78; mm('¢')= 79; 
	mm('£')= 80;  mm('¤')= 81; mm('¥')= 82; mm('€')= 83; mm('+')= 84; mm('<')= 85; mm('=')= 86; mm('>')= 87;  mm('±')= 88; mm('«')= 89; mm('»')= 90; mm('×')= 91; mm('÷')= 92; mm('§')= 93; mm('©')= 94; mm('¬')= 95; 
	mm('®')= 96;  mm('°')= 97; mm('µ')= 98; mm('¶')= 99; mm('…')=100; mm('†')=101; mm('‡')=102; mm('•')=103;  mm('•')=104; mm('‰')=105; mm('0')=106; mm('¼')=107; mm('½')=108; mm('¾')=109; mm('1')=110; mm('¹')=111; 
	mm('2')=112;  mm('²')=113; mm('3')=114; mm('³')=115; mm('4')=116; mm('5')=117; mm('6')=118; mm('7')=119;  mm('8')=120; mm('9')=121; mm('a')=122; mm('A')=123; mm('ª')=124; mm('á')=125; mm('Á')=126; mm('à')=127; 
	mm('À')=128;  mm('â')=129; mm('Â')=130; mm('ä')=131; mm('Ä')=132; mm('ã')=133; mm('Ã')=134; mm('å')=135;  mm('Å')=136; mm('æ')=137; mm('Æ')=138; mm('b')=139; mm('B')=140; mm('c')=141; mm('C')=142; mm('ç')=143; 
	mm('Ç')=144;  mm('d')=145; mm('D')=146; mm('ð')=147; mm('Ð')=148; mm('e')=149; mm('E')=150; mm('é')=151;  mm('É')=152; mm('è')=153; mm('È')=154; mm('ê')=155; mm('Ê')=156; mm('ë')=157; mm('Ë')=158; mm('f')=159; 
	mm('F')=160;  mm('ƒ')=161; mm('g')=162; mm('G')=163; mm('h')=164; mm('H')=165; mm('i')=166; mm('I')=167;  mm('í')=168; mm('Í')=169; mm('ì')=170; mm('Ì')=171; mm('î')=172; mm('Î')=173; mm('ï')=174; mm('Ï')=175; 
	mm('j')=176;  mm('J')=177; mm('k')=178; mm('K')=179; mm('l')=180; mm('L')=181; mm('m')=182; mm('M')=183;  mm('n')=184; mm('N')=185; mm('ñ')=186; mm('Ñ')=187; mm('o')=188; mm('O')=189; mm('º')=190; mm('ó')=191; 
	mm('Ó')=192;  mm('ò')=193; mm('Ò')=194; mm('ô')=195; mm('Ô')=196; mm('ö')=197; mm('Ö')=198; mm('õ')=199;  mm('Õ')=200; mm('ø')=201; mm('Ø')=202; mm('œ')=203; mm('Œ')=204; mm('p')=205; mm('P')=206; mm('q')=207; 
	mm('Q')=208;  mm('r')=209; mm('R')=210; mm('s')=211; mm('S')=212; mm('š')=213; mm('Š')=214; mm('ß')=215;  mm('t')=216; mm('T')=217; mm('þ')=218; mm('Þ')=219; mm('™')=220; mm('u')=221; mm('U')=222; mm('ú')=223; 
	mm('Ú')=224;  mm('ù')=225; mm('Ù')=226; mm('û')=227; mm('Û')=228; mm('ü')=229; mm('Ü')=230; mm('v')=231;  mm('V')=232; mm('w')=233; mm('W')=234; mm('x')=235; mm('X')=236; mm('y')=237; mm('Y')=238; mm('ý')=239; 
	mm('Ý')=240;  mm('ÿ')=241; mm('Ÿ')=242; mm('z')=243; mm('Z')=244; mm('ž')=245; mm('Ž')=246; mm('\\')=247; mm(127)=248; mm(129)=249; mm(141)=250; mm(143)=251; mm(144)=252; mm(157)=253; mm(160)=254; mm(173)=255; 
#undef mm
#define mm(_x) (map_nocase[(unsigned char)_x])
	
	mm(  0)=  0;  mm(  1)=  1; mm(  2)=  2; mm(  3)=  3; mm(  4)=  4; mm(  5)=  5; mm(  6)=  6; mm(  7)=  7;  mm(  8)=  8; mm(  9)=  9; mm( 10)= 10; mm( 11)= 11; mm( 12)= 12; mm( 13)= 13; mm( 14)= 14; mm( 15)= 15; 
	mm( 16)= 16;  mm( 17)= 17; mm( 18)= 18; mm( 19)= 19; mm( 20)= 20; mm( 21)= 21; mm( 22)= 22; mm( 23)= 23;  mm( 24)= 24; mm( 25)= 25; mm( 26)= 26; mm( 27)= 27; mm( 28)= 28; mm( 29)= 29; mm( 30)= 30; mm( 31)= 31; 
	mm('\'')= 32; mm('-')= 33; mm('–')= 34; mm('—')= 35; mm(' ')= 36; mm('!')= 37; mm('"')= 38; mm('#')= 39;  mm('$')= 40; mm('%')= 41; mm('&')= 42; mm('(')= 43; mm(')')= 44; mm('*')= 45; mm(',')= 46; mm('.')= 47; 
	mm('/')= 48;  mm(':')= 49; mm(';')= 50; mm('?')= 51; mm('@')= 52; mm('(')= 53; mm(')')= 54; mm('^')= 55;  mm('ˆ')= 56; mm('_')= 57; mm('`')= 58; mm('{')= 59; mm('|')= 60; mm('}')= 61; mm('~')= 62; mm('¡')= 63; 
	mm('¦')= 64;  mm('¨')= 65; mm('¯')= 66; mm('´')= 67; mm('¸')= 68; mm('¿')= 69; mm('˜')= 70; mm('‘')= 71;  mm('’')= 72; mm('‚')= 73; mm('“')= 74; mm('”')= 75; mm('„')= 76; mm('‹')= 77; mm('›')= 78; mm('¢')= 79; 
	mm('£')= 80;  mm('¤')= 81; mm('¥')= 82; mm('€')= 83; mm('+')= 84; mm('<')= 85; mm('=')= 86; mm('>')= 87;  mm('±')= 88; mm('«')= 89; mm('»')= 90; mm('×')= 91; mm('÷')= 92; mm('§')= 93; mm('©')= 94; mm('¬')= 95; 
	mm('®')= 96;  mm('°')= 97; mm('µ')= 98; mm('¶')= 99; mm('…')=100; mm('†')=101; mm('‡')=102; mm('•')=103;  mm('•')=104; mm('‰')=105; mm('0')=106; mm('¼')=107; mm('½')=108; mm('¾')=109; mm('1')=110; mm('¹')=111; 
	mm('2')=112;  mm('²')=113; mm('3')=114; mm('³')=115; mm('4')=116; mm('5')=117; mm('6')=118; mm('7')=119;  mm('8')=120; mm('9')=121; mm('a')=122; mm('A')=122; mm('ª')=124; mm('á')=125; mm('Á')=125; mm('à')=127; 
	mm('À')=127;  mm('â')=129; mm('Â')=129; mm('ä')=131; mm('Ä')=131; mm('ã')=133; mm('Ã')=133; mm('å')=135;  mm('Å')=135; mm('æ')=137; mm('Æ')=137; mm('b')=139; mm('B')=139; mm('c')=141; mm('C')=141; mm('ç')=143; 
	mm('Ç')=143;  mm('d')=145; mm('D')=145; mm('ð')=147; mm('Ð')=147; mm('e')=149; mm('E')=149; mm('é')=151;  mm('É')=151; mm('è')=153; mm('È')=153; mm('ê')=155; mm('Ê')=155; mm('ë')=157; mm('Ë')=157; mm('f')=159; 
	mm('F')=159;  mm('ƒ')=161; mm('g')=162; mm('G')=162; mm('h')=164; mm('H')=164; mm('i')=166; mm('I')=166;  mm('í')=168; mm('Í')=168; mm('ì')=170; mm('Ì')=170; mm('î')=172; mm('Î')=172; mm('ï')=174; mm('Ï')=174; 
	mm('j')=176;  mm('J')=176; mm('k')=178; mm('K')=178; mm('l')=180; mm('L')=180; mm('m')=182; mm('M')=182;  mm('n')=184; mm('N')=184; mm('ñ')=186; mm('Ñ')=186; mm('o')=188; mm('O')=188; mm('º')=190; mm('ó')=191; 
	mm('Ó')=191;  mm('ò')=193; mm('Ò')=193; mm('ô')=195; mm('Ô')=195; mm('ö')=197; mm('Ö')=197; mm('õ')=199;  mm('Õ')=199; mm('ø')=201; mm('Ø')=201; mm('œ')=203; mm('Œ')=203; mm('p')=205; mm('P')=205; mm('q')=207; 
	mm('Q')=207;  mm('r')=209; mm('R')=209; mm('s')=211; mm('S')=211; mm('š')=213; mm('Š')=213; mm('ß')=215;  mm('t')=216; mm('T')=216; mm('þ')=218; mm('Þ')=218; mm('™')=220; mm('u')=221; mm('U')=221; mm('ú')=223; 
	mm('Ú')=223;  mm('ù')=225; mm('Ù')=225; mm('û')=227; mm('Û')=227; mm('ü')=229; mm('Ü')=229; mm('v')=231;  mm('V')=231; mm('w')=233; mm('W')=233; mm('x')=235; mm('X')=235; mm('y')=237; mm('Y')=237; mm('ý')=239; 
	mm('Ý')=239;  mm('ÿ')=241; mm('Ÿ')=241; mm('z')=243; mm('Z')=243; mm('ž')=245; mm('Ž')=245; mm('\\')=247; mm(127)=248; mm(129)=249; mm(141)=250; mm(143)=251; mm(144)=252; mm(157)=253; mm(160)=254; mm(173)=255; 
#undef mm
}

static char 
i_str_utf8to_latin1(const char* *l) {
	char c = **l;
	if (c >= 0) 
		return c;   
	if ((c & 0x20) == 0)       {
		short u;
		u  = (c & 0x1F) << 6;    
		(*l)++;
		c = **l;
		u |= (c & 0x3F);         
		if (u >= -128 && u < 128)
			return (char)u;
		else
			return 0;
	}
	
	if ((c & 0x10) == 0)       
		*l += 3-1;  
	else if ((c & 0x08) == 0)  
		*l += 4-1;
	return 0;
}

/*
   The Alphanum Algorithm is an improved sorting algorithm for strings
   containing numbers.  Instead of sorting numbers in ASCII order like a
   standard sort, this algorithm sorts numbers in numeric order.
   The Alphanum Algorithm is discussed at http://www.DaveKoelle.com/alphanum.html
   This implementation is Copyright (c) 2008 Dirk Jagdmann <doj@cubic.org>.
   It is a cleanroom implementation of the algorithm and not derived by
   other's works. In contrast to the versions written by Dave Koelle this
   source code is distributed with the libpng/zlib license.
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
   1. The origin of this software must not be misrepresented; you
   must not claim that you wrote the original software. If you use
   this software in a product, an acknowledgment in the product
   documentation would be appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and
   must not be misrepresented as being the original software.
   3. This notice may not be removed or altered from any source
   distribution.
 ***************************************************************************
 The following code is based on the "alphanum.hpp" code 
 downloaded from the Dave Koelle page and implemented by Dirk Jagdmann.
 It was modified to the C language and simplified to PGUI needs.
 */
int 
pgui_str_compare(const char *l, const char *r, int casesensitive, int utf8) {
	enum mode_t { STRING, NUMBER } mode=STRING;
	if (l == r)
		return 0;
	if (!l && r)
		return -1;
	if (l && !r)
		return 1;
	if (!Latin1_map)
		i_str_init_latin1_map();
	while(*l && *r) {
		if (mode == STRING) {
			while((*l) && (*r)) {
				int diff;
				char l_char = *l, 
					 r_char = *r;
				
				int l_digit = pgui_isdigit(l_char), 
					r_digit = pgui_isdigit(r_char);
				
				if(l_digit && r_digit) {
					mode = NUMBER;
					break;
				}
				
				if(l_digit) return -1;
				
				if(r_digit) return +1;
				if (utf8) {
					l_char = i_str_utf8to_latin1(&l);  
					r_char = i_str_utf8to_latin1(&r);
				}
				
				if (casesensitive)
					diff = Latin1_map[(unsigned char)l_char] - Latin1_map[(unsigned char)r_char];
				else
					diff = Latin1_map_nocase[(unsigned char)l_char] - Latin1_map_nocase[(unsigned char)r_char];
				
				if(diff != 0) return diff;
				
				++l;
				++r;
			}
		}
		else {
			unsigned long r_int;
			long diff;
			
			unsigned long l_int=0;
			while(*l && pgui_isdigit(*l)) {
				
				l_int = l_int*10 + *l-'0';
				++l;
			}

			r_int=0;
			while(*r && pgui_isdigit(*r)) {
				
				r_int = r_int*10 + *r-'0';
				++r;
			}
			
			diff = l_int-r_int;
			if (diff != 0)
				return (int)diff;
			
			mode=STRING;
		}
	}
	if (*r) return -1;
	if (*l) return +1;
	return 0;
}

int 
pgui_str_compare_equal(const char *l, const char *r, int casesensitive, int utf8, int partial) {
	if (!l || !r)
		return 0;
	if (!Latin1_map)
		i_str_init_latin1_map();
	while(*l && *r) {
		int diff;
		char l_char = *l, 
			 r_char = *r;
		if (utf8) {
			l_char = i_str_utf8to_latin1(&l);  
			r_char = i_str_utf8to_latin1(&r);
		}
		
		if (casesensitive)
			diff = l_char - r_char;
		else
			diff = Latin1_map_nocase[(unsigned char)l_char] - Latin1_map_nocase[(unsigned char)r_char];
		
		if(diff != 0) 
			return 0;
		
		++l;
		++r;
	}
	
	if (*l == *r) 
		return 1;
	if (partial && *r == 0) 
		return 1;  
	return 0;
}

static int 
i_str_inc_utf8(const char* str) {
	if (*str >= 0)     
		return 1;  
	else if ((*str & 0x20) == 0)  
		return 2;  
	else if ((*str & 0x10) == 0)  
		return 3;  
	else if ((*str & 0x08) == 0)  
		return 4;
	return 1;
}

int 
pgui_str_compare_find(const char *l, const char *r, int casesensitive, int utf8) {
	int i, inc, l_len, r_len, count;
	if (!l || !r)
		return 0;
	l_len = (int)strlen(l);
	r_len = (int)strlen(r);
	count = l_len - r_len;
	if (count < 0)
		return 0;
	count++;
	for (i=0; i<count; i++) {
		if (pgui_str_compare_equal(l, r, casesensitive, utf8, 1))
			return 1;
		if (utf8) {
			inc = i_str_inc_utf8(l);
			l += inc;
			i += inc-1;
		}
		else
			l++;
	}
	return 0;
}

static void 
i_str_fix_pos_utf8(const char* str, int *start, int *end) {
	int p = 0, i = 0, find = 0, inc;
	while (*(str + i)) {
		if (find == 0 && p == *start) {
			*start = i;
			find = 1;
		}
		if (find == 1 && p == *end) {
			*end = i;
			return;
		}
		inc = i_str_inc_utf8(str + i);
		i += inc;
		p++;
	}
	if (find == 0 && p == *start) {
		*start = i;
		find = 1;
	}
	if (find == 1 && p == *end)
		*end = i;
}

void 
pgui_str_remove(char* str, int start, int end, int dir, int utf8) {
	int len;
	if (end < start || !str || str[0] == 0)
		return;
	if (start == end) {
		if (dir == 1)  
			end++;
		else  {
			if (start == 0) 
				return;
			else
				start--;
		}
	}
	if (utf8)
		i_str_fix_pos_utf8(str, &start, &end);
	
	len = (int)strlen(str); if (start >= len) { start = len - 1; end = len; }
	if (end > len) end = len;
	memmove(str + start, str + end, len - end + 1);
}

char * 
pgui_str_insert(const char* str, const char* insert_str, int start, int end, int utf8) {
	char* new_str = (char*)str;
	int insert_len, len;
	if (!str || !insert_str)
		return NULL;
	insert_len = (int)strlen(insert_str);
	len = (int)strlen(str);
	if (utf8)
		i_str_fix_pos_utf8(str, &start, &end);
	if (end == start || insert_len > end - start) {
		new_str = malloc(len - (end - start) + insert_len + 1);
		memcpy(new_str, str, start);
		memcpy(new_str + start, insert_str, insert_len);
		memcpy(new_str + start + insert_len, str + end, len - end + 1);
	}
	else {
		memcpy(new_str + start, insert_str, insert_len);
		memcpy(new_str + start + insert_len, str + end, len - end + 1);
	}
	return new_str;
}

int 
pgui_str_is_ascii(const char* str) {
	if (!str)
		return 0;
	while (*str) {
		int c = *str;
		if (c < 0)
			return 0;
		str++;
	}
	return 1;
}

static char * 
i_str_set_locale(const char* decimal_symbol) {
	if (decimal_symbol) {
		struct lconv* locale_info = localeconv();
		if (locale_info->decimal_point[0] != decimal_symbol[0]) {
			char* old_locale = setlocale(LC_NUMERIC, NULL);
			if (decimal_symbol[0] == '.') {
				old_locale = pgui_str_dup(old_locale);  
				setlocale(LC_NUMERIC, "en-US");
				return old_locale;
			}
			else if (decimal_symbol[0] == ',') {
				old_locale = pgui_str_dup(old_locale);  
				setlocale(LC_NUMERIC, "pt-BR");
				return old_locale;
			}
		}
	}
	return NULL;
}

static void 
i_str_reset_locale(char* old_locale) {
	if (old_locale) {
		setlocale(LC_NUMERIC, old_locale);
		free(old_locale);
	}
}

int 
pgui_str_to_double_locale(const char *str, double *d, const char* decimal_symbol) {
	int ret, locale_set = 0;
	char* old_locale;
	if (!str) 
		return 0;
	old_locale = i_str_set_locale(decimal_symbol);
	if (old_locale) locale_set = 1;
	ret = sscanf(str, "%lf", d);
	i_str_reset_locale(old_locale);
	if (ret != 1) 
		return 0;
	if (locale_set)
		return 2;
	else
		return 1;
}

void 
pgui_str_printf_double_locale(char *str, const char *format, double d, const char* decimal_symbol) {
	char* old_locale = i_str_set_locale(decimal_symbol);
	sprintf(str, format, d);
	i_str_reset_locale(old_locale);
}

