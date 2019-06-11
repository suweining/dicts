#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rapidjson_c.h"

using namespace std;

void *test_pool_alloc(void *arg, size_t size)
{
    (void)arg;
    std::cout<<"malloc "<<size<<std::endl;
    return (void *)malloc(size);
}

#if 0 
int main()
{
    rapidjson_writer_t w = rapidjson_new_writer(1024);

    rapidjson_start_obj(w,NULL,0);
    rapidjson_insert_string(w, "hello", sizeof("hello")-1, "//\\world", sizeof("//\\world") - 1);
    rapidjson_insert_true(w, "t", 1);
    rapidjson_insert_false(w, "f", 1);
    rapidjson_insert_null(w, "n", 1);
    rapidjson_insert_uint(w, "i", 1, 123);
    rapidjson_insert_double(w, "pi", 2, 3.1415926);
    rapidjson_insert_string(w, "\r牛排", sizeof("\r牛排")-1, "\r牛排", sizeof("\r牛排") - 1);
    

    rapidjson_start_array(w, "a", 1);
    for (unsigned i = 0; i < 6; i++)
        rapidjson_insert_uint(w, NULL, 0, i);
    rapidjson_end_array(w);
    rapidjson_end_obj(w);
    
    const char *pp = NULL;
    (void)rapidjson_writer_dump(w, &pp);
    
    std::cout << "json writer dump " << std::endl;
    std::cout << pp << std::endl;
    rapidjson_free_writer(w);

    w = rapidjson_new_writer2(test_pool_alloc, NULL, 1024);

    rapidjson_start_obj2(w,NULL,0);
    rapidjson_insert_string2(w, "hello", sizeof("hello")-1, "world", sizeof("world") - 1);
    rapidjson_insert_true2(w, "t", 1);
    rapidjson_insert_false2(w, "f", 1);
    rapidjson_insert_null2(w, "n", 1);
    rapidjson_insert_uint2(w, "i", 1, 123);
    rapidjson_insert_double2(w, "pi", 2, 3.1415926);
    rapidjson_insert_string2(w, "\r牛排", sizeof("\r牛排")-1, "\r牛排", sizeof("\r牛排") - 1);
    

    rapidjson_start_array2(w, "a", 1);
    for (unsigned i = 0; i < 6; i++)
        rapidjson_insert_uint2(w, NULL, 0, i);
    rapidjson_end_array2(w);
    rapidjson_end_obj2(w);
    
    pp = NULL;
    (void)rapidjson_writer_dump2(w, &pp);
    std::cout<< "json writer dump2 " << std::endl;
    std::cout<< pp << std::endl;
    rapidjson_free_writer2(w);
    
    //test parser 
    char b[1024];
    strcpy(b, pp);

    rapidjson_doc_t d = rapidjson_new_doc(b, strlen(b), 1);
    if(d) {
        std::cout<<"json parse ok"<<std::endl;
    }
    
    const char *s1[] = {"hello", NULL };
    rapidjson_set_string((rapidjson_value_t)d, s1, "beautiful world.", sizeof("beautiful world.") - 1, d);
    const char *s2[] = {"t", NULL };
    rapidjson_set_bool((rapidjson_value_t)d, s2, 0, d);
    const char *s3[] = {"f", NULL };
    rapidjson_set_bool((rapidjson_value_t)d, s3, 1, d);
    const char *s4[] = {"i", NULL };
    rapidjson_set_int((rapidjson_value_t)d, s4, 456, d);
    
    rapidjson_object_del_member((rapidjson_value_t)d, "n");
    const char *s5[] = {"pi", NULL };
    rapidjson_set_double((rapidjson_value_t)d, s5, 4444.2233, d);
    
    const char *s6[] = {"a", NULL };
    size_t a_size; 
    rapidjson_value_t arr = rapidjson_get_arary((rapidjson_value_t)d, s6, &a_size);
    const char *s7[] = {NULL};
    if(arr) {
        rapidjson_value_t v = rapidjson_array_add_elem(arr, StringType, d);
        //rapidjson_set_uint(v, s7, 83847392, d);
        rapidjson_set_string(v, s7, "abcde", sizeof("abcde") - 1, d);
        rapidjson_array_del_elem(arr, 2, d);
    }
    
    //object添加操作测试
    rapidjson_value_t t = rapidjson_object_add_member((rapidjson_value_t)d, "test", StringType, d); 
    rapidjson_set_string((rapidjson_value_t)t, s7, "99999999", sizeof("99999999") - 1, d);
    
    size_t str_len = 0;
    const char *kkk[] = {"hello", NULL};
    const char *nnn = rapidjson_get_string(d, kkk, &str_len);

    printf("%.*s\n", (int)str_len, nnn);

    size_t str_len2 = 0;
    const char *nnn2 = rapidjson_get_string2(d, "hello", &str_len2);
    printf("%.*s\n", (int)str_len2, nnn2);

    rapidjson_doc_dump(d, &pp);
    std::cout<< pp << std::endl;
    
    rapidjson_free_doc(d);
    return 0;
}
#endif
