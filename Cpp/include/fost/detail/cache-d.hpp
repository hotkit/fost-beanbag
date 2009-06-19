/*
    Copyright 1998-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


namespace fostlib {


    namespace detail {


        class FOST_CACHE_DECLSPEC objectcache_base : boost::noncopyable {
        public:
            virtual ~objectcache_base();
        };

        class FOST_CACHE_DECLSPEC fostcache_dbc : boost::noncopyable {
        protected:
            fostcache_dbc();
            fostcache_dbc( dbconnection &dbc );
            boost::scoped_ptr< dbconnection > m_dbc_ptr;
        };


    }


}

