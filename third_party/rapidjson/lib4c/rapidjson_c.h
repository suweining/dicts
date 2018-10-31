#ifndef RAPIDJSON_C_H_
#define RAPIDJSON_C_H_
#include <sys/types.h>
#include <stdint.h>

extern "C"
{

enum RapidJsonNumberType {
    rNullType = 0,
    rIntType = 1,
    rUintType = 2,
    rDoubleType = 3
};

//should be consistent with enum Type in rapidjson lib
enum RapidJsonType {
	NullType = 0,		//!< null
	FalseType = 1,		//!< false
	TrueType = 2,		//!< true
	ObjectType = 3,	//!< object
	ArrayType = 4,		//!< array
	StringType = 5,	//!< string
	NumberType = 6		//!< number
};

#if 0
//scalar type
enum RapidJsonScalarType {
    sNone = 0, //none scalar or nothing
    sNullType = 1,
    sBoolType = 2,
    sStringType = 3,
    sIntType = 4,
    sUintType = 5,
    sDoubleType = 6
};

typedef struct {
    const char *str;
    size_t len;
}rapidjson_str_t;

#define rapidjson_str_init(a) {a, sizeof(a)-1}
#define rapidjson_str_null {NULL, 0}

typedef union {
    int64_t i;
    uint64_t ui;
    double d;
    rapidjson_str_t s;
    unsigned b; //bool, 0:false,1:true
    unsigned n;//null
}rapidjson_scalar_t;

typedef int (*rapidjson_value_cb)(size_t index,enum RapidJsonScalarType type,rapidjson_scalar_t *value, void *arg);
#endif


typedef struct {
    enum RapidJsonNumberType type;
    union {
        int64_t i;
        uint64_t u;
        double d;
    }n;
}rapidjson_number_t;

typedef void *(*my_pool_alloc)(void *arg, size_t size);
typedef void* rapidjson_writer_t;
typedef void* rapidjson_reader_t;
typedef void* rapidjson_doc_t;
typedef void* rapidjson_value_t;

typedef int (*rapidjson_object_cb)(const char *key, size_t key_len, rapidjson_value_t value, void *arg);
typedef int (*rapidjson_array_cb)(size_t index, rapidjson_value_t value, void *arg);

rapidjson_writer_t rapidjson_new_writer(size_t init_size);
rapidjson_writer_t rapidjson_new_writer2(my_pool_alloc m_func, void *arg, size_t init_size);
void rapidjson_free_writer(rapidjson_writer_t wr);
void rapidjson_free_writer2(rapidjson_writer_t wr);
void rapidjson_start_obj(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_end_obj(rapidjson_writer_t wr);
void rapidjson_start_array(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_end_array(rapidjson_writer_t wr);
void rapidjson_insert_string(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len);
void rapidjson_insert_rawstring(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len);
void rapidjson_insert_int(rapidjson_writer_t wr, const char *key, size_t klen, int64_t val);
void rapidjson_insert_uint(rapidjson_writer_t wr, const char *key, size_t klen,  uint64_t val);
void rapidjson_insert_double(rapidjson_writer_t wr, const char *key, size_t klen, double val);
void rapidjson_insert_true(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_insert_false(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_insert_null(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_start_obj2(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_end_obj2(rapidjson_writer_t wr);
void rapidjson_start_array2(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_end_array2(rapidjson_writer_t wr);
void rapidjson_insert_string2(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len);
void rapidjson_insert_rawstring2(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len);
void rapidjson_insert_int2(rapidjson_writer_t wr, const char *key, size_t klen, int64_t val);
void rapidjson_insert_uint2(rapidjson_writer_t wr, const char *key, size_t klen,  uint64_t val);
void rapidjson_insert_double2(rapidjson_writer_t wr, const char *key, size_t klen, double val);
void rapidjson_insert_true2(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_insert_false2(rapidjson_writer_t wr, const char *key, size_t klen);
void rapidjson_insert_null2(rapidjson_writer_t wr, const char *key, size_t klen);
size_t rapidjson_writer_dump(rapidjson_writer_t wr, const char **out);
size_t rapidjson_writer_dump2(rapidjson_writer_t wr, const char **out);

rapidjson_doc_t rapidjson_new_doc(char *buffer, size_t len, int insitu);
rapidjson_doc_t rapidjson_new_empty_doc();//add by douwenxuan
void rapidjson_free_doc(rapidjson_doc_t doc);
int rapidjson_has_member(rapidjson_value_t obj, const char *key);
enum RapidJsonType rapidjson_value_get_type(rapidjson_value_t value);
rapidjson_value_t rapidjson_get_arary(rapidjson_value_t obj, const char *keys[], size_t *arr_size);
rapidjson_value_t rapidjson_array_get_elem(rapidjson_value_t array, size_t index);
rapidjson_value_t rapidjson_set_arary(rapidjson_value_t obj, const char *keys[], rapidjson_doc_t doc);
rapidjson_value_t rapidjson_array_add_elem(rapidjson_value_t array, enum RapidJsonType type, rapidjson_doc_t doc);
int rapidjson_array_del_elem(rapidjson_value_t array, rapidjson_value_t obj_to_del);
rapidjson_value_t rapidjson_set_array_elem_to_string(rapidjson_value_t array, size_t index, const char *val, rapidjson_doc_t doc); //add by douwenxuan
int rapidjson_array_foreach(rapidjson_value_t array, rapidjson_array_cb cb, void *arg);
rapidjson_value_t rapidjson_get_object(rapidjson_value_t obj, const char *keys[]);
rapidjson_value_t rapidjson_set_object(rapidjson_value_t obj, const char *keys[], rapidjson_doc_t doc);
rapidjson_value_t rapidjson_object_get_member(rapidjson_value_t obj, const char *key);
rapidjson_value_t rapidjson_object_add_member(rapidjson_value_t obj, const char *key, enum RapidJsonType type, rapidjson_doc_t doc);
int rapidjson_object_del_member(rapidjson_value_t obj, const char *key);
int rapidjson_object_foreach(rapidjson_value_t obj, rapidjson_object_cb cb, void *arg);
rapidjson_value_t rapidjson_set_string(rapidjson_value_t obj, const char *keys[], const char *val, size_t len, rapidjson_doc_t doc);
rapidjson_value_t rapidjson_set_int(rapidjson_value_t obj, const char *keys[], int64_t val, rapidjson_doc_t doc);
rapidjson_value_t rapidjson_set_uint(rapidjson_value_t obj, const char *keys[], uint64_t val, rapidjson_doc_t doc);
rapidjson_value_t rapidjson_set_double(rapidjson_value_t obj, const char *keys[], double val, rapidjson_doc_t doc);
rapidjson_value_t rapidjson_set_bool(rapidjson_value_t obj, const char *keys[], int val, rapidjson_doc_t doc);
rapidjson_number_t rapidjson_get_int(rapidjson_value_t obj, const char *keys[]);
rapidjson_number_t rapidjson_get_uint(rapidjson_value_t obj, const char *keys[]);
rapidjson_number_t rapidjson_get_double(rapidjson_value_t obj, const char *keys[]);
size_t rapidjson_doc_dump(rapidjson_doc_t doc, const char **out);
size_t rapidjson_doc_dump2(rapidjson_doc_t doc, const char **out, my_pool_alloc m_func, void *arg);
size_t rapidjson_value_dump(rapidjson_value_t value, const char **out, rapidjson_doc_t doc);//add by douwenxuan
int rapidjson_get_bool(rapidjson_value_t obj, const char *keys[]);
const char * rapidjson_get_string(rapidjson_value_t obj, const char *keys[],size_t *str_len);
const char * rapidjson_get_string2(rapidjson_value_t obj, const char *keys, size_t *str_len);
const char *rapidjson_convert_to_string(rapidjson_value_t obj, char buf[32], size_t *out_len);
}
#endif
