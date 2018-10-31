// Copyright (C) 2011 Milo Yip
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef RAPIDJSON_ONEBOXWRITER_H_
#define RAPIDJSON_ONEBOXWRITER_H_

#include "writer.h"

#ifdef __GNUC__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
#endif

namespace rapidjson {

//! Writer with indentation and spacing.
/*!
    \tparam OutputStream Type of ouptut os.
    \tparam SourceEncoding Encoding of source string.
    \tparam TargetEncoding Encoding of output stream.
    \tparam StackAllocator Type of allocator for allocating memory of stack.
*/
template<typename OutputStream, typename SourceEncoding = UTF8<>, typename TargetEncoding = UTF8<>, typename StackAllocator = CrtAllocator>
class OneboxWriter : public Writer<OutputStream, SourceEncoding, TargetEncoding, StackAllocator> {
public:
    typedef Writer<OutputStream, SourceEncoding, TargetEncoding, StackAllocator> Base;
    typedef typename Base::Ch Ch;

    //! Constructor
    /*! \param os Output stream.
        \param allocator User supplied allocator. If it is null, it will create a private one.
        \param levelDepth Initial capacity of stack.
    */
    OneboxWriter(OutputStream& os, StackAllocator* allocator = 0, size_t levelDepth = Base::kDefaultLevelDepth) : 
        Base(os, allocator, levelDepth) {}


    /*! @name Implementation of Handler
        \see Handler
    */
    //@{
    bool String(const Ch* str, SizeType length, bool copy = false) {
        (void)copy;
        this->Prefix(kStringType);
        return WriteString(str, length);
    }

    bool RawString(const Ch* str, SizeType length, bool copy = false) {
        (void)copy;
        this->Prefix(kStringType);
        Ch *dst = this->os_->Push(length);
        memcpy(dst, str ,length);
        return true;
    }

    //@}

    /*! @name Convenience extensions */
    //@{
    
    //! Simpler but slower overload.
    bool String(const Ch* str) { return String(str, internal::StrLen(str)); }
    bool RawString(const Ch* str) { return RawString(str, internal::StrLen(str)); }

    //@}
protected:
    // NOTE: '/' is escaped to cope with HTML
    bool WriteString(const Ch* str, SizeType length)  {
        static const char hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        static const char escape[256] = {
#define Z16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
            //0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
            'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'b', 't', 'n', 'u', 'f', 'r', 'u', 'u', // 00
            'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', // 10
              0,   0, '"',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '/', // 20
            Z16, Z16,                                                                       // 30~4F
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,'\\',   0,   0,   0, // 50
            Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16                                // 60~FF
#undef Z16
        };
        OutputStream* os_ = this->os_; 
        os_->Put('\"');
        GenericStringStream<SourceEncoding> is(str);
        while (is.Tell() < length) {
            const Ch c = is.Peek();
            if (!TargetEncoding::supportUnicode && (unsigned)c >= 0x80) {
                // Unicode escaping
                unsigned codepoint;
                if (!SourceEncoding::Decode(is, &codepoint))
                    return false;
                os_->Put('\\');
                os_->Put('u');
                if (codepoint <= 0xD7FF || (codepoint >= 0xE000 && codepoint <= 0xFFFF)) {
                    os_->Put(hexDigits[(codepoint >> 12) & 15]);
                    os_->Put(hexDigits[(codepoint >>  8) & 15]);
                    os_->Put(hexDigits[(codepoint >>  4) & 15]);
                    os_->Put(hexDigits[(codepoint      ) & 15]);
                }
                else if (codepoint >= 0x010000 && codepoint <= 0x10FFFF)    {
                    // Surrogate pair
                    unsigned s = codepoint - 0x010000;
                    unsigned lead = (s >> 10) + 0xD800;
                    unsigned trail = (s & 0x3FF) + 0xDC00;
                    os_->Put(hexDigits[(lead >> 12) & 15]);
                    os_->Put(hexDigits[(lead >>  8) & 15]);
                    os_->Put(hexDigits[(lead >>  4) & 15]);
                    os_->Put(hexDigits[(lead      ) & 15]);
                    os_->Put('\\');
                    os_->Put('u');
                    os_->Put(hexDigits[(trail >> 12) & 15]);
                    os_->Put(hexDigits[(trail >>  8) & 15]);
                    os_->Put(hexDigits[(trail >>  4) & 15]);
                    os_->Put(hexDigits[(trail      ) & 15]);                    
                }
                else
                    return false;   // invalid code point
            }
            else if ((sizeof(Ch) == 1 || (unsigned)c < 256) && escape[(unsigned char)c])  {
                is.Take();
                os_->Put('\\');
                os_->Put(escape[(unsigned char)c]);
                if (escape[(unsigned char)c] == 'u') {
                    os_->Put('0');
                    os_->Put('0');
                    os_->Put(hexDigits[(unsigned char)c >> 4]);
                    os_->Put(hexDigits[(unsigned char)c & 0xF]);
                }
            }
            else
                Transcoder<SourceEncoding, TargetEncoding>::Transcode(is, *os_);
        }
        os_->Put('\"');
        return true;
    } 

private:
    // Prohibit copy constructor & assignment operator.
    OneboxWriter(const OneboxWriter&);
    OneboxWriter& operator=(const OneboxWriter&);
};

} // namespace rapidjson

#ifdef __GNUC__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_RAPIDJSON_H_
