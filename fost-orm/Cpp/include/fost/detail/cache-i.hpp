/*
    Copyright 1998-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


template< typename O, typename K > inline
fostlib::object_ptr< O, K >::object_ptr() {
}

template< typename O, typename K > inline
bool fostlib::object_ptr< O, K >::operator == ( const object_ptr &r ) const {
    return m_key == r.m_key;
}
template< typename O, typename K > inline
bool fostlib::object_ptr< O, K >::operator != ( const object_ptr &r ) const {
    return m_key != r.m_key;
}

template< typename O, typename K > inline
bool fostlib::object_ptr< O, K >::isnull() const {
    return m_key.isnull();
}
template< typename O, typename K > inline
bool fostlib::object_ptr< O, K >::operator == ( fostlib::t_null ) const {
    return isnull();
}
template< typename O, typename K > inline
bool fostlib::object_ptr< O, K >::operator != ( fostlib::t_null ) const {
    return !isnull();
}

template< typename O, typename K > inline
const typename fostlib::object_ptr< O, K >::key_type &fostlib::object_ptr< O, K >::value() const {
    return m_key.value();
}

template< typename O, typename K > inline
typename fostlib::object_ptr< O, K >::instance_type *
        fostlib::object_ptr< O, K >::operator -> () {
    if ( m_key.isnull() )
        throw exceptions::null("Pointer does not contain a key");
    else
        throw exceptions::not_implemented(
            "Fetching a pointer when there is a key"
        );
}

namespace fostlib {
    template< typename O, typename K > inline
    bool operator == ( t_null, const object_ptr< O, K > &o ) {
        return o.isnull();
    }
    template< typename O, typename K > inline
    bool operator != ( t_null, const object_ptr< O, K > &o ) {
        return !o.isnull();
    }
}
