//
// Created by mike on 5/8/21.
//

#ifndef KBI_MEMBUF_H
#define KBI_MEMBUF_H

#include <streambuf>

// https://stackoverflow.com/a/57862858
struct membuf : std::streambuf {
    membuf(const char* begin, const char* end)
    {
        this->setg(const_cast<char*>(begin), const_cast<char*>(begin), const_cast<char*>(end));
    }

    pos_type
    seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override
    {
        if (dir == std::ios_base::cur)
            gbump(off);
        else if (dir == std::ios_base::end)
            setg(eback(), egptr() + off, egptr());
        else if (dir == std::ios_base::beg)
            setg(eback(), eback() + off, egptr());
        return gptr() - eback();
    }

    pos_type seekpos(pos_type sp, std::ios_base::openmode which) override
    {
        return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
    }
};

#endif //KBI_MEMBUF_H
