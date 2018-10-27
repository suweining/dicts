#ifndef __DICTS_KEY__
#define __DICTS_KEY__

class IKey {
    public:    
        virtual int Init(const void* input) = 0;
        virtual int Key(void* output) const = 0;
        virtual int ToString(void* output) const = 0;
        virtual int Compare(const IKey& k) const = 0;
        virtual int Func(const void* input, void* output) = 0;
        virtual ~IKey() {}
};

#endif
