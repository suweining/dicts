#ifndef __DICTS_VALUE__
#define __DICTS_VALUE__

class IValue {
    public:
        virtual int Init(const void* input) = 0;
        virtual int Val(void* output) = 0;
        virtual int ToString(void* output) = 0;
        virtual int Compare(const IValue& v) = 0;
        virtaul int Func(const void* input, viod* output) = 0;
        ~IValue() {}
};
#endif
