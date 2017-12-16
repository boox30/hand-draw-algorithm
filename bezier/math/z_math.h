/*
 * =====================================================================================
 *       Filename:  z_math.h
 *    Description:  
 *        Version:  2.0
 *        Created:  06/23/2016 14:53:43
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  zl(88911562@qq.com), 
 *   Organization:  
 * =====================================================================================
 */
//_________________________________
// stl on mac location is:
// /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1
// Mac OS X 10.9+ no longer uses GCC/libstdc++ but uses libc++ and Clang.
//---------------------------------
//td c++ (STL, streams, ...) : Modified libstdc++ headers for use with ctags 
//use ctags for c++(stl,streams....)
//www.vim.org/scripts/script.php?script_id=2358
//ctags -R --c++-kinds=+p --fields=+ias --extra=+q --language-force=c++ cpp_src 
//sudo ctags -R --c++-kinds=+p --fields=+ias --extra=+q --language-force=c++ ./
//================================= */
#ifndef z_math_h_
#define z_math_h_

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdint.h>

typedef struct z_point_s  z_point;
typedef struct z_fpoint_s z_fpoint;
typedef struct z_ipoint_s z_ipoint;
typedef struct z_fpoint_array_s z_fpoint_array;
typedef struct z_fpoint_arraylist_node_s  z_fpoint_arraylist_node;
typedef struct z_fpoint_arraylist_s z_fpoint_arraylist;

struct z_point_s {
	float x, y;
};

struct z_fpoint_s{
    z_point p;
    float w;
};

struct z_ipoint_s {
    z_point p;
    int64_t t;
};

struct z_fpoint_array_s {
	z_fpoint *point;
    float maxwidth;
    float minwidth;
    int ref;
	int len;
    int cap;

	z_point last_point;
	float last_width;
	int64_t last_ms;
};

struct z_fpoint_arraylist_node_s {
    z_fpoint_array *a;
    z_fpoint_arraylist_node *n;
};

struct z_fpoint_arraylist_s {
    int ref;
    z_fpoint_arraylist_node *first;
    z_fpoint_arraylist_node *end;
	z_fpoint_arraylist_node *cur;
};

z_fpoint_array *z_keep_fpoint_array(z_fpoint_array *a);
void z_drop_fpoint_array(z_fpoint_array *a);

z_fpoint_arraylist* z_keep_fpoint_arraylist(z_fpoint_arraylist *l);
void z_drop_fpoint_arraylist(z_fpoint_arraylist *l);

z_fpoint_array *z_new_fpoint_array(int initsize, float maxwidth, float minwidth);
z_fpoint_array *z_resize_fpoints_array(z_fpoint_array* a, int size);

z_fpoint_arraylist *z_new_fpoint_arraylist();
void z_fpoint_arraylist_append(z_fpoint_arraylist *l, z_fpoint_array *a);
// must be drop after used
z_fpoint_array *z_fpoint_arraylist_append_new(z_fpoint_arraylist *l, float maxwidth, float minwidth);
void z_fpoint_arraylist_removelast(z_fpoint_arraylist *l);

float z_movespeed(z_ipoint s, z_ipoint e);
float z_distance(z_point s, z_point e);
void  z_fpoint_add_xyw(z_fpoint_array *a, float x, float y, float w);
void  z_fpoint_add(z_fpoint_array *a, z_fpoint p);
void  z_fpoint_differential_add(z_fpoint_array *a, z_fpoint p);
void  z_square_bezier(z_fpoint_array *a, z_fpoint b, z_point c, z_fpoint e);
float z_linewidth(z_ipoint b, z_ipoint e, float w, float step);

float z_insert_point(z_fpoint_array *arr, z_point point);
void  z_insert_last_point(z_fpoint_array *arr, z_point e);


typedef struct z_list_node_s z_list_node;
struct z_list_node_s {
    void *data; 
    z_list_node *n;
    z_list_node *p;
}; 
typedef void*(*z_list_node_alloc_fun)();
typedef void(*z_list_node_drop_fun) (void *data);


struct z_list_s {
    z_list_node_alloc_fun alloc;
    z_list_node_drop_fun  drop;
    z_list_node *first;
    z_list_node *last;
};
typedef struct z_list_s z_list;

z_list *z_list_new(z_list_node_alloc_fun allocfun, z_list_node_drop_fun dropfun);
void *z_list_append_new(z_list *zlist);
void *z_list_remove_last(z_list *zlist);
void z_list_clear(z_list *zlist);
void z_list_free(z_list *zlist);

/* digest must be 33 char size  */
// void z_text_md5(const char* str, char *digest);

#ifdef __cplusplus
}
#endif

#endif
