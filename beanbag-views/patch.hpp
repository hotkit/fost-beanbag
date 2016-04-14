/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <beanbag/jsondb_ptr.hpp>


namespace beanbag {


    namespace patch {


        /// Transformation function that is used to build the database transactions
        using transform_fn = std::function<void(jsondb_ptr)>;

        /// Convert a JSON sequence into a set of transformations
        using transforms = std::vector<transform_fn>;


        /// Return the transforms that match the provided JSON definition
        transforms operations(const fostlib::json &);


    }


}

