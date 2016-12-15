#pragma once

namespace lain {

template <typename InputStream, typename OutputStream>
struct MakeContext
{
    typedef InputStream InputStreamType;
    typedef OutputStream OutputStreamType;
};

};

