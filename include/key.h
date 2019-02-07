#ifndef __DICTS_KEY__
#define __DICTS_KEY__

class IKey {
    public:
        virtual int Init(const void* input) = 0; // init key from readline (key \t value)
        virtual int SetKey(const void* input) = 0; // set key by key_str which shall from GetKey
        virtual int GetKey(void* output) const = 0; // get key_str
        virtual int ToString(void* output) const = 0;
        virtual int Compare(const IKey& k) const = 0;
        virtual int Func(const void* input, void* output) = 0;
        virtual ~IKey() {}

        // 希望能见名之意

};

#endif
