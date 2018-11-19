#ifndef __DICTS_KEY__
#define __DICTS_KEY__

class IKey {
    public:
        // for init the Cxxx_Key class 
        virtual int Init(const void* input) = 0;
        // set the key for match
        virtual int SetKey(const void* input) = 0;
        virtual int GetKey(void* output) const = 0;
        virtual int ToString(void* output) const = 0;
        virtual int Compare(const IKey& k) const = 0;
        virtual int Func(const void* input, void* output) = 0;
        virtual ~IKey() {}

        // 希望能见名之意

};

#endif
