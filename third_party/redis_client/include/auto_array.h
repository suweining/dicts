#ifndef __AUTO_ARRAY_H__
#define __AUTO_ARRAY_H__


//AutoArray is kind of like std::vector
//but used for the sutituation that you have known the size you need before using std::vector
//AutoArray may have better performance than std::vector

//notice : do not apply to class type, beacause of the memset after new[]
//question : how to determine whether a type is a class type or a built-in type??
template <typename T>
class AutoArray {
public:
    AutoArray() : m_array(NULL), m_size(0), m_capacity(0) {}

    ~AutoArray() {
        if (NULL != m_array) {
            delete [] m_array;
            m_array = NULL;
        }
    }

private:
    AutoArray(const AutoArray<T>&);
    AutoArray<T>& operator=(const AutoArray<T>&);

public:
    bool Init(size_t size) {
        if (NULL != m_array) {
            return false;
        }

        m_array = new (std::nothrow) T[size + 1UL];
        if (NULL == m_array) {
            return false;
        }

        memset(m_array, 0, sizeof(T) * (size  + 1UL));
        m_size = size;
        m_capacity = size + 1;

        return true;
    }

    bool Resize(size_t size) {
        if (NULL != m_array && size < m_capacity) {
            memset(m_array, 0, sizeof(T) * m_capacity);
            m_size = size;

            return true;
        }

        if (NULL != m_array) {
            delete [] m_array;
            m_array = NULL;
            m_size = 0;
            m_capacity = 0;
        }

        m_array = new (std::nothrow) T[size + 1UL];
        if (NULL == m_array) {
            return false;
        }

        memset(m_array, 0, sizeof(T) * (size  + 1UL));
        m_size = size;
        m_capacity = size + 1;

        return true;
    }

    T& operator[](size_t i) {
        if (i >= m_size) {
            return m_array[m_size];
        }

        return m_array[i];
    }

    bool Insert(size_t i, const T& item) {
        if (i >= m_size) {
            return false;
        }

        m_array[i] = item;

        return true;
    }

    bool Get(size_t i, T* item) {
        if (i >= m_size) {
            return false;
        }

        *item = m_array[i];

        return true;
    }

private:
    T* m_array;
    size_t m_size;
    size_t m_capacity;
};



#endif /* __AUTO_ARRAY_H__ */
