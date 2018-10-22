#ifndef __MMAP_WRITER__
#define __MMAP_WRITER__

#include <string>

class MmapWriter {
    public:
        MmapWriter(const char* file);
        ~MmapWriter();
        MmapWriter(const MmapWriter& mw) = delete;

        int AppendLine(const std::string& line);
        int Reset();
    private:
        void*       m_ptr;
        size_t      m_size;
        size_t      m_write_size;
};
#endif
