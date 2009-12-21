/** \defgroup fost_schema_infrastructure infrastructure
    \ingroup fost_schema

    \brief Defines the capabilities of what the persistence can do.
*/


/** \class fostlib::field_base fost/detail/schema/fields.hpp fost/factory
    \ingroup fost_schema_infrastructure

    \brief Super class for attribute field types.

    Every type that is to be used as an attribute type and persisted must have its storage requirements defined by an sub-class of fostlib::field_base.
*/
/** \fn boost::shared_ptr< meta_attribute > fostlib::field_base::meta_maker (const string &name, bool key, bool not_null, const nullable< std::size_t > &size, const nullable< std::size_t > &precision) const
    \param name The name of the attribute.
    \param key This is true if the attribute is a primary key on its model.
    \param not_null This is true if the attribute value is required.
    \param size A size parameter for the attribute.
    \param precision A precision parameter for the attribute.

    \brief Returns the attribute factory that will create the attribute itself for the required configuration.
*/
/** \fn const_iterator  fostlib::field_base::begin () const

    \brief Return an iterator for the sub-structure of the field.
*/
/** \fn const_iterator  fostlib::field_base::end () const

    \brief Return an iterator for the sub-structure of the field.
*/
/** \typedef fostlib::field_base::columns_type

    \brief Storage type for the internal structure of the field.

    Note that the internal struture is not configurable. This means that the internal structure must be fixed and cannot change depending on the configuration parameters. This is probably wrong, but works for the field types encountered so far.
*/
/** \typedef fostlib::field_base::const_iterator

    \brief An iterator type that resolves to boost::shared_ptr< meta_attribute >.

    If a field has any important internal structure then it needs to store this inside the field definition and return the meta_attributes that define it through the iterators.
*/


/** \class fostlib::field_wrapper fost/detail/schema/attributes.hpp fost/factory
    \ingroup fost_schema_infrastructure

    \brief An implementation of the fostlib::field_base interface for simple value types.

    The field_wrapper implements the storage through two nested classes, one handling the case where the attribute is required and one where the attribute is optional (nullable). Where the attribute is required the storage of the underlying type is wrapped in a fostlib::nullable.
*/
