#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include "rapidjson/finder.h"

using namespace std;
using namespace rapidjson;


//JsonPath *jp[] = {new JsonPath("$.hello\""), new JsonPath("$.a\"[*].a\""), new JsonPath("$.pi\""), new JsonPath("$.xx\".*")};

struct MyHandler: public BaseJsonPathHandler {
    bool Null(size_t idx) { cout << "Null(): " << idx <<endl; return true; }
    bool Bool(bool b, size_t idx) { cout << "Bool(" << boolalpha << b << "): " << idx << endl; return true; }
    bool Int64(int64_t i,size_t idx) { cout << "Int64(" << i << "): " << idx << endl; return true; }
    bool Uint64(uint64_t u, size_t idx) { cout << "Uint64(" << u << "): " << idx << endl; return true; }
    bool Double(double d, size_t idx) { cout << "Double(" << d << "): " << idx << endl; return true; }
    bool String(const char*str, size_t l, size_t idx) {
        (void)str;
        (void)l;
        (void)idx;
        printf("String(%.*s,%u): %u\n",(int)l,str,(unsigned int)l,(unsigned int)idx);
        return true;
    }
    bool Object(const char*str, size_t l, size_t idx) {
        printf("Object(%.*s,%u): %u\n",(int)l,str,(unsigned int)l,(unsigned int)idx);
        return true;
    }
    bool Array(const char*str, size_t l, size_t idx) {
        printf("Array(%.*s,%u): %u\n",(int)l,str,(unsigned int)l,(unsigned int)idx);
        return true;
    }
};

struct testHandler: public BaseJsonPathHandler {
    bool String(const char*str, size_t l, size_t idx) {
        (void)str;
        (void)idx;
        _len = l;
        printf("String(%.*s,%u): %u\n",(int)l,str,(unsigned int)l,(unsigned int)idx);
        return true;
    }
    size_t _len;
};


#if 1 
int main(int argc, char **argv)
{
    if(argc < 2) {
        cout<<"miss args: json_file"<<endl;
        return 0;
    }
    char json[1024*1024];
    //JsonPath *t[] = {new JsonPath("$.mini\".videoso_url\""), new JsonPath("$.long\".*"), new JsonPath("$.mini\".result\"[*].site\"")};
    ifstream f(argv[1],ios::in);
    f.read(json,1024*1024);
    size_t flen = f.gcount();
    struct timeval tv,tv2;
    int i = 1;

#if 1 
    JsonPath *t[] = {new JsonPath("$.mini.videoso_url"), new JsonPath("$.long.*"), new JsonPath("$.mini.result[*].site")};
    //JsonPath *t[] = {new JsonPath("$.long.*")};
    //JsonPath *t[] = {new JsonPath("$.mini.result[*].")};

    gettimeofday(&tv, NULL);
    while(i--) {
        GenericFinder<UTF8<>, sizeof(t)/sizeof(JsonPath *)> finder(t);
        MyHandler handler;
        //StringStream ss(json);
        MemoryStream ss(json, flen);
        finder.Parse(ss, handler);
    }

    gettimeofday(&tv2, NULL);
    cout<<(tv2.tv_sec-tv.tv_sec)*1000000+tv2.tv_usec-tv.tv_usec<<endl;
#endif   

#if 0 
    gettimeofday(&tv, NULL);
    i = 100;
    while(i--) {
        Document document;
        document.Parse(json);
        //document.ParseInsitu(json);
    }
    gettimeofday(&tv2, NULL);
    cout<<(tv2.tv_sec - tv.tv_sec)*1000000+tv2.tv_usec-tv.tv_usec<<endl;
#endif
    f.close();
    return 0;
}
#endif

inline int ishex(int x)
{
    return  (x >= '0' && x <= '9')  ||
        (x >= 'a' && x <= 'f')  ||
        (x >= 'A' && x <= 'F');
}

int decode(const char *s, char *dec)
{
    char *o;
    const char *end = s + strlen(s);
    int c;

    for (o = dec; s <= end; o++) {
        c = *s++;
        if (c == '+') c = ' ';
        else if (c == '%' && (  !ishex(*s++)    ||
                    !ishex(*s++)    ||
                    !sscanf(s - 2, "%2x", &c)))
            return -1;

        if (dec) *o = c;
    }

    return o - dec;
}

#if 0
int main(int argc, char **argv) {
    (void)argc;
    ifstream file(argv[1]);
    if(!file.is_open()) {
        cout<<"file open failed"<<endl;
        return 0;
    }
    string line;
    map<string,string> nlp_json;
    while(getline(file,line)) {
        vector<string> vec;
        line.erase(line.find_last_not_of(" \n\r\t")+1);
        std::stringstream ss(line);
        std::string item;
        while (std::getline(ss, item, '\t')) {
            vec.push_back(item);
        }
        if(vec.size() < 2) continue;
        char *dec = (char *)malloc(vec[1].length());
        decode(vec[1].c_str(), dec);
        nlp_json[vec[0]]=string(dec);
        free(dec);
    }
    JsonPath *jp[] = {new JsonPath("$.data\"[*].recreason\"")};

    struct timeval tv;
    gettimeofday(&tv, NULL);
    cout<<(tv.tv_sec*1000000 + tv.tv_usec)<<endl;
    for(map<string,string>::iterator p = nlp_json.begin(); p != nlp_json.end(); p++) {
        GenericFinder<UTF8<>, 1> finder(jp);
        testHandler handler;
        StringStream ss(p->second.c_str());
        finder.Parse(ss, handler);
    }
    gettimeofday(&tv, NULL);
    cout<<(tv.tv_sec*1000000 + tv.tv_usec)<<endl;
    return 0;
}
#endif
