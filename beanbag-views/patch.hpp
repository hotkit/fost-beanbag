/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <beanbag/jsondb_ptr.hpp>


namespace beanbag {


    namespace patch {


        /// Transformation function that is used to build the database transactions
        using transform_fn = std::function<void(fostlib::jsondb::local &)>;


        /// Register instances of these to handle individual transformations
        class transform {
          protected:
            using transform_fn = beanbag::patch::transform_fn;

            transform(fostlib::nliteral name);

          public:
            const fostlib::nliteral name;

            virtual transform_fn operator()(const fostlib::json &) const = 0;
        };


        /// Convert a JSON sequence into a set of transformations
        using transforms = std::vector<transform_fn>;


        /// Return the transforms that match the provided JSON definition
        transforms operations(const fostlib::json &);


    }


}
