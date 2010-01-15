/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_META_MODEL_HPP
#define FOST_META_MODEL_HPP
#pragma once


#include <fost/factory>


namespace fostlib {


    class FOST_METAMODEL_DECLSPEC type_descriptor : public model< type_descriptor > {
    public:
        type_descriptor( const json &j );

        FSL_ATTRIBUTE_PK( name, string );

        boost::shared_ptr< meta_instance > described_type() const;
    private:
        boost::shared_ptr< meta_instance > m_type;
    };


}


#endif // FOST_META_MODEL_HPP
