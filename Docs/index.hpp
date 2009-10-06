/** \defgroup fost_orm fost-orm

    \brief This package contains implementations of various concrete persistence layers for Fost and the abstractions that allow them to operate together.

    The database back ends are designed to work with both relational databases (RDBMSs) and non-relational databases. The back ends are also designed to be layerable, where one layer adds on the functionality of the layers below it. This library includes a simple transactional JSON database with minimum features.

    The abstraction layer is able to handle both dynamic models (i.e. the models are defined at runtime, possibly through introspection of an existing database), static models (the models are defined more or less as normal C++ classes with special annotations) and a mixture of the two (static models augmented with dynamic fields).
*/


/** \defgroup fost_cache fost-cache
    \ingroup fost_orm

    \brief Contains the persistence caching layer.
*/


/** \defgroup fost_jsondb fost-jsondb
    \ingroup fost_orm

    \brief Contains the database driver to turn the JSON database into a Fost 4 persistence back end.
*/


/** \defgroup fost_metamodel fost-metamodel
    \ingroup fost_orm

    \brief Contains a layer which can be used to describe the models that can be persisted in a back end.
*/


/** \defgroup fost_sql fost-sql
    \ingroup fost_orm

    \brief A helper library that can be used to build SQL back ends.
*/
