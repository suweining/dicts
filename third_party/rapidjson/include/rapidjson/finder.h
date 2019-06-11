#ifndef RAPIDJSON_FINDER_H_
#define RAPIDJSON_FINDER_H_

// Copyright (c) 2011 Milo Yip (miloyip@gmail.com)
// Copyright (c) 2014 Ma Bo (redshift@outlook.com)
#include <assert.h>
#include "rapidjson.h"
#include "encodings.h"
#include "internal/pow10.h"
#include "internal/stack.h"

#if defined(RAPIDJSON_SIMD) && defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#endif
#ifdef RAPIDJSON_SSE42
#include <nmmintrin.h>
#elif defined(RAPIDJSON_SSE2)
#include <emmintrin.h>
#endif

#include "error/error.h" // ParseErrorCode, ParseResult
#include "reader.h"
#include "memorystream.h"

namespace rapidjson {

//将对象数组等结构表示成特殊的key，这样可以使用统一的匹配机制
#define JSON_START "\1"
#define JSON_OBJECT_START "\"" //所有对象的key以"开始，而数组元素以"\3"开始，这样可以统一区分对象和数组元素
#define JSON_ARRAY_START "\2"

#define TAKE_NEXT_VALUE  0x80000000
struct MatchInfo{
    public:
    MatchInfo():cur(NULL),match_depth(0){};
    void *cur; //当前匹配节点
    int match_depth;
};

class AlignAllocator {
    public:
        static const bool kNeedFree = true;
        void* Malloc(size_t size) {
            char *buf;
            int ret = posix_memalign((void **)&buf, 64, size); //cache line alignment
            if (ret) {
                return NULL;
            }
            return buf;
        }
        void* Realloc(void* originalPtr, size_t originalSize, size_t newSize) { 
            void *newBuf = Malloc(newSize);
            if(originalPtr) memcpy(newBuf, originalPtr, originalSize<newSize?originalSize:newSize); 
            std::free(originalPtr);
            return newBuf; 
        }
        static void Free(void *ptr) { std::free(ptr); }
};

class JsonPath {
    public:
    JsonPath(const char *json_path):
        json_expr_(json_path),
        stack_(NULL, kDefaultStackCapacity)
    {
        expr_to_dfa(json_path);
    }
    ~JsonPath(){}
    void *json_get_start()
    {
        return (void *)start; 
    }
    
    // 返回值：期望的下一级key的depth，0表示全部完成，后续无需匹配
    // 如果对象存在重复的key，只查看第一个key，后续忽略
    unsigned char json_match(MatchInfo *m, const char *str, size_t str_len, int cur_depth) {
        State *cur = (State *)m->cur;
        
        //如果当前depth小于期望depth，本节点已经遍历完毕，如果前面没有通配符节点，
        //则后续没必要再查看，直接返回完成
        if(cur_depth < (int)cur->depth) {
            //m->match_depth = 0;
            while(cur->parent != 0) {
                if(cur->len == sizeof(State) + 1 && cur->depth <= cur_depth)
                    break;
                cur = (State *)((char *)cur - cur->parent);
            }
            m->cur = cur;
            if(cur->parent == 0) {
                return 0;
            }
        }

        //如果当前json的depth和我期望的不一致，略过
        if(cur_depth != (int)cur->depth) {
            return cur->depth;
        }
        //m->match_depth = 0;

        //如果wild card匹配或者精确匹配，进入下一级
        if((cur->len == sizeof(State) + 1 && cur->key[0] == str[0]) 
           || (cur->len == str_len + sizeof(State) && memcmp(cur->key, str, str_len) == 0))
        {
            m->cur = cur = (State *)((char *)cur + cur->len);
            if(cur->len == sizeof(State)) { 
                //完全匹配成功，标记下个值应当被处理
                m->match_depth = (int)cur->depth;
                //如果有通配符节点，定位到上一个通配符节点
                while(cur->parent != 0) {
                    if(cur->len == sizeof(State) + 1)
                        break;
                   cur = (State *)((char *)cur - cur->parent);
                }
                m->cur = cur;
                return cur->depth;
            }
            return  cur->depth; 
        }
        else {
            //匹配失败，
            cur = cur;
            return cur->depth; 
        }
    }

    private:
    //状态标识
    struct State {
        unsigned char len; //total length of this struct
        unsigned char parent; //parent state offset before this state
        unsigned char depth; //depth from root to this node
        char key[0];
    }__attribute__ ((aligned (1)));
    //禁用默认构造和拷贝构造
    JsonPath();
    JsonPath(const JsonPath&);
    JsonPath& operator=(const JsonPath&);
     
    size_t alloc_state(unsigned char depth, size_t last) {
        State *s = (State *)stack_.Push<char>(sizeof(State));
        size_t cur;
        s->len = sizeof(State);
        s->depth = depth;
        cur = (char *)s - (char *)stack_.Bottom<char>();
        s->parent = (unsigned char)(cur - last);

        return cur;
    }

    void expr_to_dfa(const char *expr) {
        const char *p;
        char *tmp;
        size_t s = 0;
        size_t last = 0;
        unsigned char depth = 0;

        assert(expr != NULL && *expr == '$');

        for(p=expr;*p; p++){
            switch(*p){
                default: //普通字符
                    tmp = (char *)stack_.Push<char>(1);
                    *tmp = *p;
                    break;
                case '$':
                    s = alloc_state(depth++, last);
                    last = s;
                    tmp = (char *)stack_.Push<char>(1);
                    *tmp = JSON_START[0];
                    break;
                case '.':
                    s = alloc_state(depth++, last);
                    last = s;
                    tmp = (char *)stack_.Push<char>(1);
                    *tmp = JSON_OBJECT_START[0];
                    break;
                case '[':
                    s = alloc_state(depth++, last);
                    last = s;
                    tmp = (char *)stack_.Push<char>(1);
                    *tmp = JSON_ARRAY_START[0];
                    break;
                case ']':
                case '*':
                    tmp = NULL;
                    break;
            }
            
            if(tmp != NULL) {
                State *tt = (State *)((unsigned char *)stack_.Bottom<char>() + s);
                tt->len++;
                //最多长度255，不能溢出
                assert(tt->len >= sizeof(State));
            }
            tmp = NULL;
            //最大深度255，不能溢出
            assert(depth > 0);
        }
        //设置end state标识状态结束
        s = alloc_state(depth-1, last);
        this->start = (State *)stack_.Bottom<char>();
    }
    static const size_t kDefaultStackCapacity = 128; //!< Default stack capacity in bytes for storing internal states 
    const char *json_expr_;
    State *start;
	internal::Stack<AlignAllocator> stack_;	//!< A stack for storing internal states.
};

//GenericFinder的使用者需要定义BaseJsonPathHandler的继承类，用来
//处理某个JsonPath中遇到的事件，idx是JsonPath的索引
struct BaseJsonPathHandler {
	bool Default() { return true; }
	bool Null(size_t idx) { (void)idx; return Default(); }
	bool Bool(bool v,size_t idx) { (void)v; (void)idx; return Default(); }
	bool Int64(int64_t v,size_t idx) { (void)v; (void)idx; return Default(); }
	bool Uint64(uint64_t v,size_t idx) { (void)v; (void)idx; return Default(); }
	bool Double(double v,size_t idx) { (void)v; (void)idx; return Default(); }
	bool String(const char*v, size_t l, size_t idx) { (void)v; (void)idx; (void)l; return Default(); }
    bool Object(const char*v, size_t l, size_t idx) { (void)v; (void)idx; (void)l; return Default(); }
    bool Array(const char*v, size_t l, size_t idx) { (void)v; (void)idx; (void)l; return Default(); }
};

///////////////////////////////////////////////////////////////////////////////
// GenericFinder

//! SAX-style JSON Element Finder. Use \ref Finder for UTF8 encoding and default allocator.
/*! GenericFinder parses JSON text from a stream, and send events synchronously to an 
    object which has subscribed .
*/
#define CCASSERT(exp, var) typedef char var[2*((exp)!=0)-1];
template <typename SourceEncoding, size_t JsonPathNum>
class GenericFinder {
public:
	typedef typename SourceEncoding::Ch Ch; //!< SourceEncoding character type
    CCASSERT(JsonPathNum  <= 32 && JsonPathNum > 0, ccassert_bufsize); //!< max to 32 jsonPath in one pass

	//! Constructor.
	GenericFinder(JsonPath *paths[]) : jpTodo(JsonPathNum), depth(0), parseResult_()
    {
        for(size_t i=0; i<JsonPathNum; i++) {
            jp_[i] = paths[i];
            mi_[i].cur = jp_[i]->json_get_start(); 
        }
    }

	//! Parse JSON text.
	/*! 
		\tparam InputStream Type of input stream, implementing Stream concept.
		\tparam Handler Type of handler, implementing Handler concept.
		\param is Input stream to be parsed.
		\param handler The handler to receive events.
		\return Whether the parsing is successful.
	*/
	template <typename InputStream, typename Handler>
	ParseResult Parse(InputStream& is, Handler& handler) {

		parseResult_.Clear();

		//ClearStackOnExit scope(*this);

		SkipWhitespace(is);

		if (is.Peek() == '\0') {
			RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorDocumentEmpty, is.Tell());
			RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);
		}
		else {
            for(size_t i = 0; i<JsonPathNum; i++) 
                jp_[i]->json_match(&mi_[i], JSON_START, 1, depth);

			switch (is.Peek()) {
				case '{': ParseObject(is, handler); break;
				case '[': ParseArray(is, handler); break;
				default: RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorTermination, is.Tell());
			}
			RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);

            SkipWhitespace(is);

            if (is.Peek() != '\0') {
                RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorDocumentRootNotSingular, is.Tell());
                RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);
            }
		}

		return parseResult_;
	}

	//! Whether a parse error has occured in the last parsing.
	bool HasParseError() const { return parseResult_.IsError(); }
	
	//! Get the \ref ParseErrorCode of last parsing.
	ParseErrorCode GetParseErrorCode() const { return parseResult_.Code(); }

	//! Get the position of last parsing error in input, 0 otherwise.
	size_t GetErrorOffset() const { return parseResult_.Offset(); }

protected:
    void SetParseError(ParseErrorCode code, size_t offset) { parseResult_.Set(code, offset); }

private:
	// Prohibit copy constructor & assignment operator.
	GenericFinder(const GenericFinder&);
	GenericFinder& operator=(const GenericFinder&);

    inline void HandleJsonPath(const char *str, size_t str_len, int depth) {
        unsigned int t = 0; 
        for(size_t i=0; i< JsonPathNum; i++) {
            t += jp_[i]->json_match(&mi_[i],str,str_len, depth)?1:0;
        }
        jpTodo = t;
    }

	// Parse object: { string : value, ... }
	template<typename InputStream, typename Handler>
	void ParseObject(InputStream& is, Handler& handler) {
		RAPIDJSON_ASSERT(is.Peek() == '{');
        const Ch *obj_start = is.src_;
		is.Take();	// Skip '{'
        depth++;

		SkipWhitespace(is);

		if (is.Peek() == '}') {
			is.Take();
		    depth--;

            return;
		}

		for (SizeType memberCount = 0;;) {
			if (is.Peek() != '"')
				RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissName, is.Tell());
            
            //key
            const Ch *str = NULL;
            SizeType len = 0;
			ParseString(is, &str, len);
			RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
            HandleJsonPath(str, len, depth);

			SkipWhitespace(is);

			if (is.Take() != ':')
				RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissColon, is.Tell());

			SkipWhitespace(is);

			ParseValue(is, handler);
			RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

			SkipWhitespace(is);

			++memberCount;

			switch (is.Take()) {
				case ',': SkipWhitespace(is); break;
				case '}':
                {
                    depth--;
                    for(size_t i=0; i<JsonPathNum; i++) {
                        if(mi_[i].match_depth == depth)  {
                            handler.Object(obj_start, is.src_ - obj_start, i);
                            mi_[i].match_depth = 0;
                        }
                    }
                    return;
                }
				default:  RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissCommaOrCurlyBracket, is.Tell());
			}
		}
	}

	// Parse array: [ value, ... ]
	template<typename InputStream, typename Handler>
	void ParseArray(InputStream& is, Handler& handler) {
		RAPIDJSON_ASSERT(is.Peek() == '[');
		const Ch *array_start = is.src_;
        is.Take();	// Skip '['
        depth++; 
		
		SkipWhitespace(is);

		if (is.Peek() == ']') {
			is.Take();
            depth--;
			return;
		}
       
        //数组的key以JSON_ARRAY_START开头，这样可以区分出对象的key
        static const char *_indexs[16] = {JSON_ARRAY_START"0",JSON_ARRAY_START"1",JSON_ARRAY_START"2",JSON_ARRAY_START"3",JSON_ARRAY_START"4",JSON_ARRAY_START"5", JSON_ARRAY_START"6",JSON_ARRAY_START"7",
                                         JSON_ARRAY_START"8",JSON_ARRAY_START"9",JSON_ARRAY_START"10",JSON_ARRAY_START"11",JSON_ARRAY_START"12",JSON_ARRAY_START"13",JSON_ARRAY_START"14",JSON_ARRAY_START"15"};
        static const size_t _lens[16] = {2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3};


		for (SizeType elementCount = 0;;) {
            if(elementCount < 16) {
                HandleJsonPath(_indexs[elementCount], _lens[elementCount], depth);
            }
            else {
                char buf[32]="";
                sprintf(buf,JSON_ARRAY_START"%u",elementCount);
                HandleJsonPath(buf, strlen(buf), depth);
            }
            
			ParseValue(is, handler);
			RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

			++elementCount;
			SkipWhitespace(is);

			switch (is.Take()) {
				case ',': SkipWhitespace(is); break;
				case ']': 
                {
                    depth--;
                    for(size_t i=0; i<JsonPathNum; i++) {
                        if(mi_[i].match_depth == depth)  {
                            handler.Array(array_start, is.src_ - array_start, i);
                            mi_[i].match_depth = 0;
                        }
                    }
                    return;
                }
				default:  RAPIDJSON_PARSE_ERROR(kParseErrorArrayMissCommaOrSquareBracket, is.Tell());
			}
		}
	}

	template<typename InputStream, typename Handler>
	void ParseNull(InputStream& is, Handler& handler) {
		RAPIDJSON_ASSERT(is.Peek() == 'n');
		is.Take();

		if (is.Take() == 'u' && is.Take() == 'l' && is.Take() == 'l') {
            for(size_t i=0; i<JsonPathNum; i++) {
                if(mi_[i].match_depth == depth)  {
                    handler.Null(i);
                    mi_[i].match_depth = 0;
                }
            }
		}
		else
			RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, is.Tell() - 1);
	}

	template<typename InputStream, typename Handler>
	void ParseTrue(InputStream& is, Handler& handler) {
		RAPIDJSON_ASSERT(is.Peek() == 't');
		is.Take();

		if (is.Take() == 'r' && is.Take() == 'u' && is.Take() == 'e') {
            for(size_t i=0; i<JsonPathNum; i++) {
                if(mi_[i].match_depth == depth)  {
                    handler.Bool(true,i);
                    mi_[i].match_depth = 0;
                }
            }
		}
		else
			RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, is.Tell() - 1);
	}

	template<typename InputStream, typename Handler>
	void ParseFalse(InputStream& is, Handler& handler) {
		RAPIDJSON_ASSERT(is.Peek() == 'f');
		is.Take();

		if (is.Take() == 'a' && is.Take() == 'l' && is.Take() == 's' && is.Take() == 'e') {
            for(size_t i=0; i<JsonPathNum; i++) {
                if(mi_[i].match_depth == depth)  {
                    handler.Bool(false,i);
                    mi_[i].match_depth = 0;
                }
            }
        }
		else
			RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, is.Tell() - 1);
	}

	// Parse string and generate String event.
	template<typename InputStream>
	void ParseString(InputStream& is, const Ch **s, SizeType &len) {
#define Z16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        static const char escape[256] = {
            Z16, Z16, 0, 0,'\"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,'/', 
            Z16, Z16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,'\\', 0, 0, 0, 
            0, 0,'\b', 0, 0, 0,'\f', 0, 0, 0, 0, 0, 0, 0,'\n', 0, 
            0, 0,'\r', 0,'\t', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16
        };
#undef Z16
        
		RAPIDJSON_ASSERT(is.Peek() == '\"');
        const Ch *start = is.src_;
		is.Take();	// Skip '\"'

		for (;;) {
			Ch c = is.Peek();
            if (c == '\\') {
               is.Take();
               //take next,here we won't check its validity 
               Ch e = is.Take();
               (void)e;
               (void)escape;
            }
            else if (c == '"') {	// Closing double quote
                *s = start;
                len = is.src_ - start;
				is.Take();
				return;
			}
			else if (c == '\0')
				RAPIDJSON_PARSE_ERROR(kParseErrorStringMissQuotationMark, is.Tell() - 1);
			else if ((unsigned)c < 0x20) // RFC 4627: unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
				RAPIDJSON_PARSE_ERROR(kParseErrorStringEscapeInvalid, is.Tell() - 1);
            else
                is.Take();
		}

        RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
	}
	
    template<typename InputStream, typename Handler>
	void ParseValueString(InputStream& is, Handler &handler) {
        
		RAPIDJSON_ASSERT(is.Peek() == '\"');
		is.Take();	// Skip '\"'
        const Ch *start = is.src_;

		for (;;) {
			Ch c = is.Peek();
            if (c == '\\') {
               is.Take();
               //take next,here we won't check its validity 
               Ch e = is.Take();
               (void)e;
            }
            else if (c == '"') {	// Closing double quote
                for(size_t i=0; i<JsonPathNum; i++) {
                    if(mi_[i].match_depth == depth)  {
                        handler.String(start, is.src_-start, i);
                        mi_[i].match_depth = 0;
                    }
                }
				is.Take();
				return;
			}
			else if (c == '\0')
				RAPIDJSON_PARSE_ERROR(kParseErrorStringMissQuotationMark, is.Tell() - 1);
			else if ((unsigned)c < 0x20) // RFC 4627: unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
				RAPIDJSON_PARSE_ERROR(kParseErrorStringEscapeInvalid, is.Tell() - 1);
            else
                is.Take();
		}

        RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
	}

    inline double StrtodFastPath(double significand, int exp) {
        // Fast path only works on limited range of values.
        // But for simplicity and performance, currently only implement this.
        // see http://www.exploringbinary.com/fast-path-decimal-to-floating-point-conversion/
        if (exp < -308)
            return 0.0;
        else if (exp >= 0)
            return significand * internal::Pow10(exp);
        else
            return significand / internal::Pow10(-exp);
    }

    template<typename InputStream, typename Handler>
        void ParseNumber(InputStream& is, Handler& handler) {
            internal::StreamLocalCopy<InputStream> copy(is);
            InputStream& s(copy.s);

            // Parse minus
            bool minus = false;
            if (s.Peek() == '-') {
                minus = true;
                s.Take();
            }

            // Parse int: zero / ( digit1-9 *DIGIT )
            unsigned i = 0;
            uint64_t i64 = 0;
            bool use64bit = false;
            if (s.Peek() == '0') {
                i = 0;
                s.Take();
            }
            else if (s.Peek() >= '1' && s.Peek() <= '9') {
                i = static_cast<unsigned>(s.Take() - '0');

                if (minus)
                    while (s.Peek() >= '0' && s.Peek() <= '9') {
                        if (i >= 214748364) { // 2^31 = 2147483648
                            if (i != 214748364 || s.Peek() > '8') {
                                i64 = i;
                                use64bit = true;
                                break;
                            }
                        }
                        i = i * 10 + static_cast<unsigned>(s.Take() - '0');
                    }
                else
                    while (s.Peek() >= '0' && s.Peek() <= '9') {
                        if (i >= 429496729) { // 2^32 - 1 = 4294967295
                            if (i != 429496729 || s.Peek() > '5') {
                                i64 = i;
                                use64bit = true;
                                break;
                            }
                        }
                        i = i * 10 + static_cast<unsigned>(s.Take() - '0');
                    }
            }
            else
                RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, s.Tell());

            // Parse 64bit int
            double d = 0.0;
            bool useDouble = false;
            if (use64bit) {
                if (minus) 
                    while (s.Peek() >= '0' && s.Peek() <= '9') {                    
                        if (i64 >= RAPIDJSON_UINT64_C2(0x0CCCCCCC, 0xCCCCCCCC)) // 2^63 = 9223372036854775808
                            if (i64 != RAPIDJSON_UINT64_C2(0x0CCCCCCC, 0xCCCCCCCC) || s.Peek() > '8') {
                                d = (double)i64;
                                useDouble = true;
                                break;
                            }
                        i64 = i64 * 10 + static_cast<unsigned>(s.Take() - '0');
                    }
                else
                    while (s.Peek() >= '0' && s.Peek() <= '9') {                    
                        if (i64 >= RAPIDJSON_UINT64_C2(0x19999999, 0x99999999)) // 2^64 - 1 = 18446744073709551615
                            if (i64 != RAPIDJSON_UINT64_C2(0x19999999, 0x99999999) || s.Peek() > '5') {
                                d = (double)i64;
                                useDouble = true;
                                break;
                            }
                        i64 = i64 * 10 + static_cast<unsigned>(s.Take() - '0');
                    }
            }

            // Force double for big integer
            if (useDouble) {
                while (s.Peek() >= '0' && s.Peek() <= '9') {
                    if (d >= 1.7976931348623157e307) // DBL_MAX / 10.0
                        RAPIDJSON_PARSE_ERROR(kParseErrorNumberTooBig, s.Tell());
                    d = d * 10 + (s.Take() - '0');
                }
            }

            // Parse frac = decimal-point 1*DIGIT
            int expFrac = 0;
            if (s.Peek() == '.') {
                s.Take();

#if RAPIDJSON_64BIT
                // Use i64 to store significand in 64-bit architecture
                if (!useDouble) {
                    if (!use64bit)
                        i64 = i;

                    while (s.Peek() >= '0' && s.Peek() <= '9') {
                        if (i64 >= RAPIDJSON_UINT64_C2(0x19999999, 0x99999999))
                            break;
                        else {
                            i64 = i64 * 10 + static_cast<unsigned>(s.Take() - '0');
                            --expFrac;
                        }
                    }

                    d = (double)i64;
                }
#else
                // Use double to store significand in 32-bit architecture
                if (!useDouble)
                    d = use64bit ? (double)i64 : (double)i;
#endif
                useDouble = true;

                while (s.Peek() >= '0' && s.Peek() <= '9') {
                    d = d * 10 + (s.Take() - '0');
                    --expFrac;
                }

                if (expFrac == 0)
                    RAPIDJSON_PARSE_ERROR(kParseErrorNumberMissFraction, s.Tell());
            }

            // Parse exp = e [ minus / plus ] 1*DIGIT
            int exp = 0;
            if (s.Peek() == 'e' || s.Peek() == 'E') {
                if (!useDouble) {
                    d = use64bit ? (double)i64 : (double)i;
                    useDouble = true;
                }
                s.Take();

                bool expMinus = false;
                if (s.Peek() == '+')
                    s.Take();
                else if (s.Peek() == '-') {
                    s.Take();
                    expMinus = true;
                }

                if (s.Peek() >= '0' && s.Peek() <= '9') {
                    exp = s.Take() - '0';
                    while (s.Peek() >= '0' && s.Peek() <= '9') {
                        exp = exp * 10 + (s.Take() - '0');
                        if (exp > 308 && !expMinus) // exp > 308 should be rare, so it should be checked first.
                            RAPIDJSON_PARSE_ERROR(kParseErrorNumberTooBig, s.Tell());
                    }
                }
                else
                    RAPIDJSON_PARSE_ERROR(kParseErrorNumberMissExponent, s.Tell());

                if (expMinus)
                    exp = -exp;
            }

            // Finish parsing, call event according to the type of number.
            bool cont = true;
            if (useDouble) {
                int expSum = exp + expFrac;
                if (expSum < -308) {
                    // Prevent expSum < -308, making Pow10(expSum) = 0
                    d = StrtodFastPath(d, exp);
                    d = StrtodFastPath(d, expFrac);
                }
                else
                    d = StrtodFastPath(d, expSum);
                for(size_t i=0; i<JsonPathNum; i++) {
                   if(mi_[i].match_depth == depth)  {
                       handler.Double(minus ? -d : d, i);
                       mi_[i].match_depth = 0;
                   }
                }
            }
            else {
                //32bit also convet to 64bit
                if(!use64bit) i64 = (uint64_t)i;

                for(size_t i=0; i<JsonPathNum; i++) {
                   if(mi_[i].match_depth == depth)  {
                       if(minus) handler.Int64(-(int64_t)i64, i);
                       else handler.Uint64(i64, i);
                       mi_[i].match_depth = 0;
                   }
                }
            }
            if (!cont)
                RAPIDJSON_PARSE_ERROR(kParseErrorTermination, s.Tell());
        }

    // Parse any JSON value
    template<typename InputStream, typename Handler>
        void ParseValue(InputStream& is, Handler& handler) {
            switch (is.Peek()) {
                case 'n': ParseNull(is, handler); break;
                case 't': ParseTrue(is, handler); break;
                case 'f': ParseFalse(is, handler); break;
                case '"': ParseValueString(is, handler); break;
                case '{': ParseObject(is, handler); break;
                case '[': ParseArray(is, handler); break;
                default : ParseNumber(is, handler);
            }
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
            //all value have beed parsed
            if(jpTodo == 0) {
                RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
            }
        }
    JsonPath *jp_[JsonPathNum];
    MatchInfo mi_[JsonPathNum];
    unsigned int jpTodo;
    int depth;
    ParseResult parseResult_;
}; // class GenericFinder

//! Finder with UTF8 encoding and default allocator.
//typedef GenericFinder<UTF8<> > Finder;

} // namespace rapidjson

#ifdef _MSC_VER
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_FINDER_H_
