#include <iostream>
#include <sstream>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "mmap_reader.h"


MmapReader::MmapReader(const char* file) : m_ptr(NULL), m_size(0), m_read_size(0) {

    int fd = open(file, O_RDONLY);

    if(-1 == fd) {
        std::cerr << "open file:" << file << " error:" << strerror(errno) << std::endl; 
        return ;
    }

    struct stat file_stat;
    if(int rc = fstat(fd, &file_stat) && -1 == rc) {
        std::cerr << "fstat file:" << file << " error:" << strerror(errno) << std::endl; 
        return ;
    }

    m_size = file_stat.st_size;

    m_ptr = mmap(NULL, m_size, PROT_READ, MAP_SHARED, fd, 0);
    if((void*) -1 == m_ptr) {
        m_ptr = NULL;
        std::cerr << "mmap file:" << file << " error:" << strerror(errno) << std::endl; 
    }
    close(fd);

    m_iss.str((char*)m_ptr);

    return ;
}

MmapReader::~MmapReader() {
    if((void*) -1 == m_ptr || NULL == m_ptr) {
        return ; 
    }

    if(int rc = munmap(m_ptr, m_size) && rc == -1) {
        std::cerr << "munmap error:" << strerror(errno) << std::endl; 
        return ; 
    }
}

int MmapReader::NextLine(std::string* line) {
    
    if(!m_iss.good()) {
        return 0; 
    }

    if(NULL == line) {
        return 0; 
    }

    std::getline(m_iss, *line);
    if(m_iss.good() || m_iss.eof()) {
        m_read_size += line->size();
    }
    else {
        return 0;
    }
    return line->size();
}

int MmapReader::Reset() {
    if((char*)-1 != (char*)m_ptr && NULL != m_ptr) {
        m_iss.str((char*)m_ptr); 
        m_read_size = 0;
    }
    return 0;
}
