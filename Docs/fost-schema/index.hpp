/** \defgroup fost_schema fost-schema
    \ingroup fost_orm

    \brief fost-schema is the lowest most layer of the OR/M part of Fost 4. It is responsible for handling the abstractions of the databases themselves.

    The database abstraction is able to handle both RDBMS systems as well as more exotic database options. This library includes a simple transactional JSON database with minimum features.

    The abstraction layer is able to handle both dynamic models (i.e. the models are defined at runtime, possibly through introspection of an existing database), static models (the models are defined more or less as normal C++ classes with special annotations) and a mixture of the two (static models augmented with dynamic fields).
*/

/** \class fostlib::model
    \ingroup fost_schema

    The model attributes are built up in several layers each handling a different aspect of the storage and management.
*/
