#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/oneboxwriter.h" // onebox customized json writer
#include "rapidjson/stringbuffer.h"
#include "rapidjson/memorystream.h"
#include "rapidjson/encodings.h"
#include "rapidjson/error/en.h"
#include <cstdio>
#include <assert.h>
#include <iostream>
#include "rapidjson_c.h"

using namespace rapidjson;

struct InsituMemoryStream {
    typedef char Ch; // byte

    InsituMemoryStream(Ch *src, size_t size) : src_(src), begin_(src), end_(src + size), dst_(0), size_(size) {}

    Ch Peek() const { return (src_ == end_) ? '\0' : *src_; }
    Ch Take() { return (src_ == end_) ? '\0' : *src_++; }
    size_t Tell() const { return static_cast<size_t>(src_ - begin_); }

    // Write
    void Put(Ch c) { RAPIDJSON_ASSERT(dst_ != 0); *dst_++ = c; }

    Ch* PutBegin() { return dst_ = src_; }
    size_t PutEnd(Ch* begin) { return static_cast<size_t>(dst_ - begin); }
    void Flush() {}

    Ch* Push(size_t count) { Ch* begin = dst_; dst_ += count; return begin; }
    void Pop(size_t count) { dst_ -= count; }

    // For encoding detection only.
    const Ch* Peek4() const {
        return Tell() + 4 <= size_ ? src_ : 0;
    }

    Ch* src_;     //!< Current read position.
    Ch* begin_;   //!< Original head of the string.
    Ch* end_;     //!< End of stream.
    Ch* dst_;     //!< Current write position
    size_t size_;       //!< Size of the stream.
};

//用户自定义内存池，内存池由用户自行维护和销毁，这里只关心malloc函数
class UserPoolAllocator {
    public:
        static const bool kNeedFree = false;
        UserPoolAllocator() { assert(0); }; //正常不应当调用此处
        UserPoolAllocator(my_pool_alloc m, void *arg): _arg(arg), _malloc_func(m) {};
        ~UserPoolAllocator() {};
        void* Malloc(size_t size) { return _malloc_func(_arg, size); }
        void* Realloc(void* originalPtr, size_t originalSize, size_t newSize)
        {
            if (originalPtr == 0)
                return Malloc(newSize);

            if (originalSize >= newSize)
                return originalPtr;
            void *dstPtr =  Malloc(newSize);
            RAPIDJSON_ASSERT(dstPtr != 0);
            memcpy(dstPtr, originalPtr, originalSize);
            return dstPtr;
        }
        static void Free(void *ptr) { (void)ptr; }
    private:
        void *_arg;
        my_pool_alloc _malloc_func;
};

typedef GenericStringBuffer<UTF8<>, UserPoolAllocator> UserPoolBuffer;
typedef OneboxWriter<StringBuffer, UTF8<>, UTF8<> > UTF8Writer;
typedef OneboxWriter<UserPoolBuffer, UTF8<>, UTF8<> > UserUTF8Writer;


class rapidjson_internal_t {
    public:
        StringBuffer sb_;
        UTF8Writer writer_;
        rapidjson_internal_t(size_t init_size) : sb_(0, init_size), writer_(sb_) { };
};

class rapidjson_internal2_t {
    public:
        UserPoolAllocator upa_;
        UserPoolBuffer ub_;
        UserUTF8Writer writer_;
        rapidjson_internal2_t(my_pool_alloc m_func, void *arg, size_t init_size) : upa_(m_func, arg), ub_(&upa_, init_size), writer_(ub_) { };
};


/*
 *  json编码相关函数，两种风格
 *  一种是用rapidjson内置的内存池进行内存分配的
 *  第二种使用用户自定义内存池进行内存分配，直接将结果编码到用户内存池，避免内存拷贝
 * */

rapidjson_writer_t rapidjson_new_writer(size_t init_size)
{
    return (rapidjson_writer_t)(new rapidjson_internal_t(init_size));
}

void rapidjson_free_writer(rapidjson_writer_t wr)
{
    delete (rapidjson_internal_t *)wr;
}

rapidjson_writer_t rapidjson_new_writer2(my_pool_alloc m_func, void *arg, size_t init_size)
{
    return (rapidjson_writer_t)(new rapidjson_internal2_t(m_func, arg, init_size));
}

void rapidjson_free_writer2(rapidjson_writer_t wr)
{
    delete (rapidjson_internal2_t *)wr;
}

void rapidjson_start_obj(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->StartObject();
}

void rapidjson_end_obj(rapidjson_writer_t wr)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;

    writer->EndObject();
}

void rapidjson_start_array(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;

    if(key) writer->String(key, klen);
    writer->StartArray();
}

void rapidjson_end_array(rapidjson_writer_t wr)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    writer->EndArray();
}


void rapidjson_insert_string(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->String(val, len);
}

//won't escape value
void rapidjson_insert_rawstring(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->RawString(val, len);
}

void rapidjson_insert_int(rapidjson_writer_t wr, const char *key, size_t klen, int64_t val)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Int64(val);
}

void rapidjson_insert_uint(rapidjson_writer_t wr, const char *key, size_t klen,  uint64_t val)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Uint64(val);
}

void rapidjson_insert_double(rapidjson_writer_t wr, const char *key, size_t klen, double val)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Double(val);
}

void rapidjson_insert_true(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Bool(true);
}

void rapidjson_insert_false(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Bool(false);
}

void rapidjson_insert_null(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UTF8Writer *writer = &((rapidjson_internal_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Null();
}

void rapidjson_start_obj2(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;

    if(key) writer->String(key, klen);
    writer->StartObject();
}

void rapidjson_end_obj2(rapidjson_writer_t wr)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;

    writer->EndObject();
}

void rapidjson_start_array2(rapidjson_writer_t wr,const char *key, size_t klen)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;

    if(key) writer->String(key, klen);
    writer->StartArray();
}

void rapidjson_end_array2(rapidjson_writer_t wr)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;

    writer->EndArray();
}


void rapidjson_insert_string2(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->String(val, len);
}

// won't insert string
void rapidjson_insert_rawstring2(rapidjson_writer_t wr, const char *key,size_t klen, const char *val,size_t len)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->RawString(val, len);
}

void rapidjson_insert_int2(rapidjson_writer_t wr, const char *key, size_t klen, int64_t val)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Int64(val);
}

void rapidjson_insert_uint2(rapidjson_writer_t wr, const char *key, size_t klen,  uint64_t val)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Uint64(val);
}

void rapidjson_insert_double2(rapidjson_writer_t wr, const char *key, size_t klen, double val)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Double(val);
}

void rapidjson_insert_true2(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Bool(true);
}

void rapidjson_insert_false2(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Bool(false);
}

void rapidjson_insert_null2(rapidjson_writer_t wr, const char *key, size_t klen)
{
    UserUTF8Writer *writer = &((rapidjson_internal2_t *)wr)->writer_;
    if(key) writer->String(key, klen);
    writer->Null();
}

size_t rapidjson_writer_dump(rapidjson_writer_t wr, const char **out)
{
    StringBuffer *buffer = &((rapidjson_internal_t *)wr)->sb_;
    *out = (const char*)buffer->GetString();
    return buffer->GetSize();
}

size_t rapidjson_writer_dump2(rapidjson_writer_t wr, const char **out)
{
    UserPoolBuffer *buffer = &((rapidjson_internal2_t *)wr)->ub_;
    *out = (const char*)buffer->GetString();
    return buffer->GetSize();
}

/*
*  DOM Parser API,当需要修改数据，或者查看的逻辑比较复杂时，使用这套API
*/
//rapidjson中Document实际是Value的子类，所以后续以Value为参数的操作都支持Document
//insitu代表是否在原始buffer上解码，而不是复制新的
rapidjson_doc_t rapidjson_new_doc(char *buffer, size_t len, int insitu)
{
    if(buffer == NULL)
        return NULL;

    Document *d = (new Document());
    if(insitu) {
        InsituMemoryStream s(buffer, len);
        d->ParseStream<kParseDefaultFlags|kParseInsituFlag, UTF8<>, InsituMemoryStream>(s);
    }
    else {
        MemoryStream s(buffer, len);
        d->ParseStream<kParseDefaultFlags, UTF8<>, MemoryStream>(s);
    }

    if(d->HasParseError())
    {
        std::cout << GetParseError_En(d->GetParseError()) << "@" << d->GetErrorOffset() << std::endl;
        delete d;
        return NULL;
    }

    return (rapidjson_doc_t)d;
}

rapidjson_doc_t rapidjson_new_empty_doc() {
  Document *doc = new Document();
  doc->SetObject();
  return (rapidjson_doc_t)doc;
}

void rapidjson_free_doc(rapidjson_doc_t doc)
{
    delete (Document *)doc;
}

int rapidjson_has_member(rapidjson_value_t obj, const char *key) {
  Value *v = (Value *)obj;
  return v->HasMember(key);
}

enum RapidJsonType rapidjson_value_get_type(rapidjson_value_t value)
{
    Value *v = (Value *)value;
    return (enum RapidJsonType)v->GetType();
}

static inline Value* _get_value(Value *v, const char *keys[])
{
    int i;

    for(i=0; keys[i] != NULL; i++) {
        if(!v->IsObject())
            break;
        Value::MemberIterator itr = v->FindMember(keys[i]);
        if (itr != v->MemberEnd()) {
            v = &itr->value;
        }
        else
            break;
    }
    if(keys[i] != NULL)
        return NULL;

    return v;
}

static inline Value* _get_value2(Value *v, const char *keys)
{
    if(!v->IsObject())
        return NULL;
    Value::MemberIterator itr = v->FindMember(keys);
    if (itr != v->MemberEnd()) {
        v = &itr->value;
        return v;
    }
    else
        return NULL;
}

static inline Value* _set_value(Value *v, const char *keys[], RapidJsonType type, Document *d)
{
    int i;

    for(i=0; keys[i] != NULL; i++) {
        if(!v->IsObject())
            break;
        Value::MemberIterator itr = v->FindMember(keys[i]);
        if (itr != v->MemberEnd()) {
            v = &itr->value;
        }
        else {
            //最后一个，且该value不存在，使用指定的type
            Value obj(keys[i+1] == NULL?(enum Type)type:kObjectType);
            v = &v->AddMember(StringRef(keys[i]), obj, d->GetAllocator());
        }
    }

    if(keys[i] != NULL)
        return NULL;

    return v;
}

rapidjson_value_t rapidjson_get_arary(rapidjson_value_t obj, const char *keys[], size_t *arr_size)
{
    Value *v = (Value *)obj;

    v = _get_value(v, keys);

    if(v == NULL || !v->IsArray())
        return NULL;
    *arr_size = v->Size();
    return (rapidjson_value_t)v;
}

//index must less than array size
rapidjson_value_t rapidjson_array_get_elem(rapidjson_value_t array, size_t index)
{
    Value *v = (Value *)array;
    if(!v->IsArray())
        return NULL;
    return (rapidjson_value_t)(&v->operator[](SizeType(index)));
}

rapidjson_value_t rapidjson_set_arary(rapidjson_value_t obj, const char *keys[], rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;

    v = _set_value(v, keys, ArrayType,(Document *)doc);
    //如果已经是array了，直接返回
    if(v == NULL || v->IsArray())
        return v;
    return &v->SetArray();
}

rapidjson_value_t rapidjson_array_add_elem(rapidjson_value_t array, RapidJsonType type, rapidjson_doc_t doc)
{
    Value *v = (Value *)array;
    if(!v->IsArray())
        return NULL;

    Value a((enum Type)type);
    v->PushBack(a, ((Document *)doc)->GetAllocator());
    return &v->operator [](v->Size() - 1);
}

//edit by douwenxuan
int rapidjson_array_del_elem(rapidjson_value_t array, rapidjson_value_t obj_to_del)
{
    Value *v = (Value *)array;
    if (!v->IsArray()) {
        return -1;      
    }
    for (Value::ConstValueIterator iter = v->Begin(); iter != v->End(); ++iter) {
        if ((&(*iter)) == (Value *)obj_to_del) {
            v->Erase(iter);
            return 0;
        }
    }
    return -1;
}

//add by douwenxuan
rapidjson_value_t rapidjson_set_array_elem_to_string(rapidjson_value_t array, size_t index,const char *val, rapidjson_doc_t doc) {
  if (array == NULL || val == NULL || doc == NULL) {
    return NULL;
  }
  Value *v = (Value *)array;
  if (!v->IsArray() || index >= v->Size()) {
    return NULL;
  }
  rapidjson_value_t obj = rapidjson_array_get_elem(array, index);
  if (obj == NULL) {
    return NULL;
  }
  return &((Value *)obj)->SetString(val, ((Document *)doc)->GetAllocator());
}

int rapidjson_array_foreach(rapidjson_value_t array, rapidjson_array_cb cb, void *arg)
{
    int i = 0;
    int ret;
    Value *d = (Value *)array;
    if(!d->IsArray())
        return 0;

    for (Value::ValueIterator itr = d->Begin();
            itr != d->End(); ++itr)
    {
        ret = cb((size_t)i, (rapidjson_value_t)(itr), arg);
        i++;
        if(ret) //done
            break;
    }
    return i;
}


rapidjson_value_t rapidjson_get_object(rapidjson_value_t obj, const char *keys[])
{
    Value *v = (Value *)obj;

    v = _get_value(v, keys);

    if(v == NULL || !v->IsObject())
        return NULL;
    return (rapidjson_value_t)v;
}

rapidjson_value_t rapidjson_set_object(rapidjson_value_t obj, const char *keys[], rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;

    v = _set_value(v, keys, ObjectType, (Document *)doc);
    //如果已经是array了，直接返回
    if(v == NULL || v->IsObject())
        return v;
    return &v->SetObject();
}

rapidjson_value_t rapidjson_object_get_member(rapidjson_value_t obj, const char *key)
{
    Value *v = (Value *)obj;
    if(!v->IsObject())
        return NULL;
    Value::MemberIterator itr = v->FindMember(key);
    if (itr != v->MemberEnd()) {
        v = &itr->value;
    }
    else
        v = NULL;

    return (rapidjson_value_t)v;
}

//返回添加完后的value，这样用户可以继续给这个Value赋值
rapidjson_value_t rapidjson_object_add_member(rapidjson_value_t obj, const char *key, RapidJsonType type, rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;
    if(!v->IsObject())
        return NULL;

    Value a((enum Type)type);
    v->AddMember(StringRef(key), a, ((Document *)doc)->GetAllocator());
    Value::MemberIterator mi = v->MemberEnd();
    --mi;
    return &(mi->value);
}

int rapidjson_object_del_member(rapidjson_value_t obj, const char *key)
{
    Value *v = (Value *)obj;
    if(!v->IsObject())
        return -2;
    return v->RemoveMember(key)?0:-1;
}

int rapidjson_object_foreach(rapidjson_value_t obj, rapidjson_object_cb cb, void *arg)
{
    int i = 0;
    int ret;
    Value *d = (Value *)obj;
    if(!d->IsObject())
        return 0;

    for (Value::MemberIterator itr = d->MemberBegin();
            itr != d->MemberEnd(); ++itr)
    {
        ret = cb(itr->name.GetString(), itr->name.GetStringLength(), (rapidjson_value_t)&itr->value, arg);
        i++;
        if(ret) //done
            break;
    }
    return i;
}

const char * rapidjson_get_string(rapidjson_value_t obj, const char *keys[],size_t *str_len)
{
    Value *v = (Value *)obj;

    v = _get_value(v, keys);

    if(v == NULL || !v->IsString())
        return NULL;
    *str_len = v->GetStringLength();
    return (const char *)(v->GetString());
}

const char * rapidjson_get_string2(rapidjson_value_t obj, const char *keys, size_t *str_len)
{
    Value *v = (Value *)obj;

    v = _get_value2(v, keys);

    if(v == NULL || !v->IsString())
        return NULL;
    *str_len = v->GetStringLength();
    return (const char *)(v->GetString());
}

rapidjson_value_t rapidjson_set_string(rapidjson_value_t obj, const char *keys[], const char *val, size_t len, rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;

    v = _set_value(v, keys, StringType,(Document *)doc);
    if(v == NULL || !v->IsString()){
        //std::cout<<"not a string to set: type " << v->GetType() << std::endl;
        return NULL;
    }
    return &v->SetString(val, len, ((Document *)doc)->GetAllocator());
}

rapidjson_number_t rapidjson_get_int(rapidjson_value_t obj, const char *keys[])
{
    Value *v = (Value *)obj;
    rapidjson_number_t t = {rNullType, {0}};

    v = _get_value(v, keys);

    if(v == NULL || !v->IsInt64())
        return t;
    t.type = rIntType;
    t.n.i = v->GetInt64();
    return t;
}

rapidjson_value_t rapidjson_set_int(rapidjson_value_t obj, const char *keys[], int64_t val, rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;

    v = _set_value(v, keys, NumberType, (Document *)doc);
    if(v == NULL || !v->IsInt64())
        return NULL;
    return &v->SetInt64(val);
}

rapidjson_number_t rapidjson_get_uint(rapidjson_value_t obj, const char *keys[])
{
    Value *v = (Value *)obj;
    rapidjson_number_t t = {rNullType, {0}};

    v = _get_value(v, keys);

    if(v == NULL || !v->IsUint64())
        return t;
    t.type = rUintType;
    t.n.u = v->GetUint64();
    return t;
}

rapidjson_value_t rapidjson_set_uint(rapidjson_value_t obj, const char *keys[], uint64_t val, rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;

    v = _set_value(v, keys, NumberType, (Document *)doc);
    if(v == NULL || !v->IsUint64())
        return NULL;
    return &v->SetUint64(val);
}

rapidjson_number_t rapidjson_get_double(rapidjson_value_t obj, const char *keys[])
{
    Value *v = (Value *)obj;
    rapidjson_number_t t = {rNullType, {0}};

    v = _get_value(v, keys);

    if(v == NULL || !v->IsDouble())
        return t;
    t.type = rDoubleType;
    t.n.d = v->GetDouble();
    return t;
}

rapidjson_value_t rapidjson_set_double(rapidjson_value_t obj, const char *keys[], double val, rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;

    v = _set_value(v, keys, NumberType, (Document *)doc);
    if(v == NULL || !v->IsDouble())
        return NULL;
    return &v->SetDouble(val);
}

//0:false, 1:true, -1:can't get
int rapidjson_get_bool(rapidjson_value_t obj, const char *keys[])
{
    Value *v = (Value *)obj;

    v = _get_value(v, keys);

    if(v == NULL || !v->IsBool())
        return -1;
    return v->GetBool()?1:0;
}

rapidjson_value_t rapidjson_set_bool(rapidjson_value_t obj, const char *keys[], int val, rapidjson_doc_t doc)
{
    Value *v = (Value *)obj;

    v = _set_value(v, keys, val?TrueType:FalseType, (Document *)doc);
    if(v == NULL || !v->IsBool())
        return NULL;
    return &v->SetBool(val?true:false);
}

const char *rapidjson_convert_to_string(rapidjson_value_t obj, char buf[32], size_t *out_len)
{
    Value *v = (Value *)obj;
    enum rapidjson::Type type = v->GetType();
    switch(type) {
        case kNullType:
        *out_len = 4;
        return "null";

        case kFalseType:
        *out_len = 5;
        return "false";

        case kTrueType:
        *out_len = 4;
        return "true";

        case kObjectType:
        case kArrayType:
        default:
        *out_len = 0;
        return NULL; //indicate error

        case kStringType:
        *out_len = v->GetStringLength();
        return v->GetString();

        case kNumberType:
        char *end = buf;
        if (v->IsInt())         end = internal::i32toa(v->GetInt(), buf);
        else if (v->IsUint())      end = internal::u32toa(v->GetUint(), buf);
        else if (v->IsInt64())     end = internal::i64toa(v->GetInt64(), buf);
        else if (v->IsUint64())    end = internal::u64toa(v->GetUint64(), buf);
        else                    end = internal::dtoa(v->GetDouble(), buf);
        *end = '\0';
        *out_len = end - buf;
        return buf;
    }
}

size_t rapidjson_doc_dump(rapidjson_doc_t doc, const char **out)
{
    //StringBuffer必须使用Document的内存池，这样dump出来的内容生存期和Document一样
    GenericStringBuffer<UTF8<>, MemoryPoolAllocator<> > sb_(&((Document *)doc)->GetAllocator());
    OneboxWriter<GenericStringBuffer<UTF8<>, MemoryPoolAllocator<> >, UTF8<>, UTF8<> > writer_(sb_);

    ((Document *)doc)->Accept(writer_);
    *out = (const char*)sb_.GetString();
    return sb_.GetSize();
}

size_t rapidjson_doc_dump2(rapidjson_doc_t doc, const char **out, my_pool_alloc m_func, void *arg)
{
    //用户自定义内存分配，用户自己管理啥时候释放
    UserPoolAllocator upa_(m_func, arg);
    UserPoolBuffer ub_(&upa_);
    UserUTF8Writer writer_(ub_);

    ((Document *)doc)->Accept(writer_);
    *out = (const char*)ub_.GetString();
    return ub_.GetSize();
}
size_t rapidjson_value_dump(rapidjson_value_t value, const char **out, rapidjson_doc_t doc) {
  //StringBuffer必须使用Document的内存池，这样dump出来的内容生存期和Document一样
  GenericStringBuffer<UTF8<>, MemoryPoolAllocator<> > sb_(&((Document *)doc)->GetAllocator());
  OneboxWriter<GenericStringBuffer<UTF8<>, MemoryPoolAllocator<> >, UTF8<>, UTF8<> > writer_(sb_);

  ((Value *)value)->Accept(writer_);
  *out = (const char*)sb_.GetString();
  return sb_.GetSize();
}
