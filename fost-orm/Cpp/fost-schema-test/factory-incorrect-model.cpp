/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/factory>


class BasicModel : public fostlib::model< BasicModel > {
public:
    BasicModel( const fostlib::json &j )
    : model_type( j ){}
};

class SubModelType : public BasicModel {
};

fostlib::factory< BasicModel > s_BasicModel;
fostlib::factory< SubModelType > s_SubModelType;

