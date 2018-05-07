/*
    Copyright 2009-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_META_MODEL_HPP
#define FOST_META_MODEL_HPP
#pragma once


#include <fost/factory>


namespace fostlib {


    /// A model that describes a type
    class FOST_METAMODEL_DECLSPEC type_descriptor
            : public model< type_descriptor > {
        boost::shared_ptr< meta_instance > m_type;
        public:
            /// Construct the descriptor
            type_descriptor( const initialiser &j );

            /// The primary key is the name
            FSL_ATTRIBUTE_PK( name, string );

            /// Provide the type that is described
            boost::shared_ptr< meta_instance > described_type() const;
    };


}


#endif // FOST_META_MODEL_HPP
