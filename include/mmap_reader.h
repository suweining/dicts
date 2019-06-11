#ifndef __MMAP_READER__
#define __MMAP_READER__

#include <string>

class MmapReader {

    public:
        MmapReader(const char* file);
        ~MmapReader();
        MmapReader(const MmapReader& mr) = delete;

        inline size_t Size() {return m_size;}
        inline size_t ReadSize() {return m_read_size;}
        int NextLine(std::string* line);
        int Reset();

    private:
        void*               m_ptr;
        size_t              m_size;
        size_t              m_read_size;
        std::istringstream  m_iss;
};

#endif
