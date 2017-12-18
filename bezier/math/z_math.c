/*
 *
 * =====================================================================================
 *       Filename:  z_math.c
 *    Description:
 *        Version:  1.0
 *        Created:  06/23/2016 14:53:43
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  zl(88911562@qq.com), 
 *   Organization:  
 * =====================================================================================
 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 
#include "z_math.h"

#define z_malloc_struct(t) (t*)calloc(1, sizeof(t))
static void* z_malloc_array(unsigned int count, unsigned int size);
static void* z_resize_array(void *p, size_t count, size_t size);

static void z_fpoint_array_set_last_info(z_fpoint_array *arr, z_point last_point, float last_width);

/***************************** mac stdlib location:
Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include/stdio.h
*/
static float z_square(float f){ return (float)f*f; };
static float z_cubic_(float f){ return (float)powf(f, 3); };

typedef struct z_bezier_factors_s {
    float bezier_step;      // must be divisible by 1.0f
	float max_width_diff;   // max width diff between two near lines
    float max_move_speed;   // 
    float max_linewith;     
} z_bezier_factors ;

int z_point_equals(z_point *p1, z_point *p2) {
    return (p1->x==p2->x&&p1->y==p2->y) ? 1 : 0;
}

z_fpoint_array *z_keep_fpoint_array(z_fpoint_array *a) {
    if(a) a->ref++;
    return a;
}

void z_drop_fpoint_array(z_fpoint_array *a) {
    if(!a) return;

    if( !(--(a->ref)) ) {
        free(a);
    }
}

z_fpoint_arraylist *z_keep_fpoint_arraylist(z_fpoint_arraylist *l) {
    if(!l) return NULL;
    l->ref++;
    return l;
}

void z_drop_fpoint_arraylist(z_fpoint_arraylist *l) {
    if(!l) return;

    if( !(--(l->ref)) ) {
        z_fpoint_arraylist_node *c = l->first;
        z_fpoint_arraylist_node *n;
        while(c) {
            z_drop_fpoint_array(c->a);
            n = c->n;
            free(c);
            c = n;
        }
    } 
}

static const float defualt_max_width = 5.0f;
static const float default_min_width = 1.0f;

z_fpoint_array *z_new_fpoint_array(int initsize, float maxwidth, float minwidth) {
    if(initsize<=0) return NULL;
    z_fpoint_array *a = malloc(sizeof(z_fpoint_array));
    a->point = z_malloc_array(initsize, sizeof(z_fpoint));
    a->ref = 1;
    a->len = 0;

    if(maxwidth<0 || minwidth<0 || maxwidth<minwidth ){
        maxwidth = defualt_max_width;
        minwidth = default_min_width;
    }

    a->maxwidth = maxwidth;
    a->minwidth = minwidth;

    a->cap = initsize;
    return a;
}

z_fpoint_array *z_resize_fpoints_array(z_fpoint_array* a, int count){
    if(!a || count<=0) return NULL;

    a->point = (z_fpoint*)z_resize_array(a->point, count, sizeof(z_fpoint));
    a->cap = count;
    a->len = min(a->cap, a->len);
    return a;
}

z_fpoint_arraylist *z_new_fpoint_arraylist() {
    z_fpoint_arraylist *l = z_malloc_struct(z_fpoint_arraylist);
    l->ref = 1;
    l->first = l->end = NULL;
    return l;
}

void z_fpoint_arraylist_append(z_fpoint_arraylist *l, z_fpoint_array *a) {
    z_fpoint_arraylist_node *node = z_malloc_struct(z_fpoint_arraylist_node);
    node->a = z_keep_fpoint_array(a);
    node->n = NULL;

    if(!l->first) {
        l->first = node;
    }
    else {
        l->end->n = node;
    }

    l->end = node;
}

z_fpoint_array *z_fpoint_arraylist_append_new(z_fpoint_arraylist *l, float max, float min) {
    z_fpoint_array *a = z_new_fpoint_array(24, max, min);
    z_fpoint_arraylist_append(l, a);
	printf("append new points array\n");
    return a; 
}

void z_fpoint_arraylist_removelast(z_fpoint_arraylist *l) {
    
    z_fpoint_arraylist_node *c = l->first;

    z_drop_fpoint_array(l->end->a);
    free(l->end);

    while(c->n != l->end) { c = c->n; }

    c->n = NULL;
    l->end = c; 
}

z_fpoint_array *z_auto_increase_fpoints_array(z_fpoint_array *a) {
    int cap = a->cap + (a->cap+3)/4;
    return z_resize_fpoints_array(a, cap);
}

float z_movespeed(z_ipoint s, z_ipoint e) {
	float d = z_distance(s.p, e.p);
    return (0==d) ? 0 : d/(e.t-s.t);
}

float z_distance(z_point b, z_point e){
    return (float)sqrtf( z_square(e.x-b.x) + z_square(e.y-b.y) );
}

void  z_fpoint_add_xyw(z_fpoint_array *a, float x, float y, float w)  {
	if( !a || (a->point[a->len-1].p.x==x && a->point[a->len-1].p.y==y) ) return;
	
    if(a->len==a->cap) 
        z_auto_increase_fpoints_array(a);

	z_fpoint *p = a->point + (a->len++);
	p->p.x = x; p->p.y = y; p->w = w;
}

void  z_fpoint_add(z_fpoint_array *a, z_fpoint p) {
    z_fpoint_add_xyw(a, p.p.x, p.p.y, p.w);
}

void  z_fpoint_differential_add(z_fpoint_array *a, z_fpoint p) {
    if(!a) return; 

    if( a->len==0 ) {
        z_fpoint_add(a, p);
        return;
    }

// #define bad_show
#ifdef bad_show
    z_fpoint_add(a, p);
	return;
#endif
	float max_diff = 0.1f;
    z_fpoint *last = a->point + (a->len-1);
    z_point sp = last->p;
	float sw = last->w;
	
	int n = (int)((fabsf(p.w - last->w) / max_diff) + 1);
    float x_step = (p.p.x - sp.x) / n;
    float y_step = (p.p.y - sp.y) / n;
    float w_step = (p.w - sw)	  / n;
	
    int i;
    for(i=0; i<(n-1); i++ ){
		sp.x += x_step;
		sp.y += y_step;
		sw += w_step;
        z_fpoint_add_xyw(a, sp.x, sp.y, sw);
    }
	z_fpoint_add(a, p);
}

void  z_square_bezier(z_fpoint_array *a, z_fpoint b, z_point c, z_fpoint e){
	if(!a) return;
	const float f = 0.1f;
	for(float t=f; t<=1.0; t+=f ) {
		float x1 = z_square(1-t)*b.p.x + 2*t*(1-t)*c.x + z_square(t)*e.p.x;
		float y1 = z_square(1-t)*b.p.y + 2*t*(1-t)*c.y + z_square(t)*e.p.y;
		float w = b.w + (t* (e.w-b.w));
		z_fpoint pw = { {x1, y1}, w};
		z_fpoint_differential_add(a, pw);
	}
}

float z_linewidth(z_ipoint b, z_ipoint e, float bwidth, float step) {
	const float max_speed = 2.0f;
	float d = z_distance(b.p, e.p);
	float s = d / (e.t - b.t); s = s > max_speed ? max_speed : s;
	float w = (max_speed-s) / max_speed;
	float max_dif = d * step;
	if( w<0.05f ) w = 0.05f;
	if( fabs( w-bwidth ) > max_dif ) {
		if( w > bwidth )
			w = bwidth + max_dif;
		else
			w = bwidth - max_dif;
	}
	// printf("d:%.4f, time_diff:%lld, speed:%.4f, width:%.4f\n", d, e.t-b.t, s, w);
	return w;
}


float z_insert_point(z_fpoint_array *arr, z_point point) {

	if(!arr) return 0;
    int len = arr->len;

	z_point zp = {point.x, point.y};
	if( 0==len ){
		z_fpoint p = {zp, 0.4f};
        z_fpoint_add(arr, p); 
		z_fpoint_array_set_last_info(arr, point, p.w);
		return p.w;
	}

	int64_t cur_ms = clock();
	float last_width = arr->last_width;
	int64_t last_ms = arr->last_ms;
	z_point last_point = arr->last_point;

	printf("cur_ms - last_ms = 0x%llx\n", cur_ms - last_ms);
	// 两次采样时间小于25毫秒, 或者距离小于2个像素, 不采样计算!!!  
	float distance = z_distance(point, last_point);
	if( (cur_ms-last_ms) < 50 || distance < 3) {
		return 0;
	}

	float step = arr->len > 4 ? 0.05f : 0.2f;
	z_ipoint bt = { {last_point.x,last_point.y}, last_ms};
	z_ipoint et = { zp, cur_ms};
	float w = (z_linewidth(bt, et, last_width, step) + last_width) / 2;
	z_fpoint_array *points = z_new_fpoint_array(51, arr->maxwidth, arr->minwidth);
    z_fpoint tmppoint = arr->point[len-1];
	z_fpoint_add(points, tmppoint);

	if( 1==len ) {
		z_fpoint p = { {(bt.p.x + et.p.x + 1) / 2, (bt.p.y + et.p.y +1) / 2}, w};
        z_fpoint_differential_add(points, p);
		w = p.w;
	}
	else {
		z_fpoint bw = tmppoint;
		z_point c =  {last_point.x,last_point.y};
		z_fpoint ew = {{(last_point.x + point.x)/2, (last_point.y + point.y)/2}, w};
		z_square_bezier(points, bw, c, ew);
	}
	
	// escape the first point
    int i;
	for(i=1; i<points->len; i++) {
        z_fpoint_add(arr, points->point[i]);
	}

	z_drop_fpoint_array(points); 
	z_fpoint_array_set_last_info(arr, point, w);

	return w;
}

void z_insert_last_point(z_fpoint_array *arr, z_point e) {
	if(!arr) return;
	long len= arr->len;
	if(len==0 ) return;
	z_fpoint_array *points = z_new_fpoint_array(51, arr->maxwidth, arr->minwidth);
	z_fpoint zb = arr->point[len-1];
	z_fpoint_add(points, zb);
	
	z_fpoint ze = { {e.x, e.y}, 0.1f};
    z_fpoint_differential_add(points, ze);
    int i;
	for(i=1; i<points->len; i++) {
        z_fpoint_add(arr, points->point[i]);
	}
	z_drop_fpoint_array(points);
}

z_list *z_list_new(z_list_node_alloc_fun allocfun, z_list_node_drop_fun dropfun)
{
    z_list *l = NULL;
	l = z_malloc_struct(z_list);
	l->alloc = allocfun;
	l->drop = dropfun;
	l->first = l->last = NULL;
    return l;
}

void *z_list_append_new(z_list *zlist) 
{
    z_list_node *node = NULL;
    void *data = NULL;

    if(!zlist->alloc || !zlist->drop) 
        return NULL;

	node = z_malloc_struct(z_list_node);
	node->data = zlist->alloc();
	node->n = NULL;
	node->p = NULL;

    if(node) {
        if(!zlist->first) {
            zlist->first = zlist->last = node;
        }
        else {
            node->n = NULL;
            node->p = zlist->last;
            zlist->last->n = node; 
            zlist->last = node;
        } 
        data = node->data;
    }

    return data;
}
void *z_list_remove_last(z_list *zlist) 
{
    void *data = NULL;
    z_list_node *tmp = zlist->last;
    if(zlist->last) {
        tmp = zlist->last;
        if(zlist->last==zlist->first){
            zlist->last = zlist->first = NULL;
        }
        else {
            zlist->last = tmp->p;
            zlist->last->n = NULL;
        }
    }

    if(tmp) {
        data = tmp->data; 
        free(tmp);
    }

    return data; 
}

void z_list_clear(z_list *zlist) 
{
	while (zlist->first)
		zlist->drop(z_list_remove_last(zlist));
}

void z_list_free(z_list *zlist) 
{
	z_list_clear(zlist);
	free(zlist);
}

/* digest must be 33 char size  */
// void
// z_text_md5(const char* str, char *digest)
// {
//     int len = strlen(str);
//     unsigned char d[16];
// 	fz_md5 state;
// 	fz_md5_init(&state);
// 	fz_md5_update(&state, (const unsigned char*)str, len);
// 	fz_md5_final(&state, d);
// 
//     int i;
//     for(i=0; i<(int)sizeof(d); i++) {
//         sprintf(digest, "%02x", d[i]);
//         digest+=2;
//     }
//     *digest = '\0';
// }

void* z_malloc_array(unsigned int count, unsigned int size) { 
	unsigned int totalsize = count * size;
	if (totalsize <= 0) return 0;

	void *buffer = malloc(count * size);
	if(buffer) memset(buffer, 0, count * size);
	return buffer;
}

void* z_resize_array(void *p, size_t count, size_t size) {
	void *np = 0;
	size_t total_size = count * size;

	if (total_size <= 0) 
		return np; 

	np = realloc(p, total_size);

	return np;
}

void z_fpoint_array_set_last_info(z_fpoint_array *arr, z_point last_point, float last_width) {
	if (!arr) return;
	arr->last_point = last_point;
	arr->last_ms = clock();
	arr->last_width = last_width; 
	printf("reset last ms to 0x%llx\n", arr->last_ms);
}

