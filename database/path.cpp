#include "path.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Path::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS paths (id SERIAL,"
                        << "begin VARCHAR(256) NOT NULL,"
                        << "end VARCHAR(256) NOT NULL);", now;
        }

        catch (Poco::Data::PostgreSQL::PostgreSQLException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::ConnectionFailedException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Path::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        root->set("begin", _begin);
        root->set("end", _end);
        return root;
    }

    Path Path::fromJSON(const std::string &str)
    {
        Path path;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        path.id() = object->getValue<long>("id");
        path.begin() = object->getValue<std::string>("begin");
        path.end() = object->getValue<std::string>("end");

        return path;
    }

    std::optional<Path> Path::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Path a;
            select << "SELECT id, begin, end FROM paths where id=$1",
                into(a._id),
                into(a._begin),
                into(a._end),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return a;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::vector<Path> Path::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Path> result;
            Path a;
            select << "SELECT id, begin, end FROM paths",
                into(a._id),
                into(a._begin),
                into(a._end),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<Path> Path::search(std::string begin)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Path> result;
            Path a;
            begin += "%";
            select << "SELECT id, begin, end FROM paths where begin LIKE $1",
                into(a._id),
                into(a._begin),
                into(a._end),
                use(begin),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Path::save_to_db()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO paths (begin,end) VALUES($1, $2)",
                use(_begin),
                use(_end);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LASTVAL()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    long Path::get_id() const
    {
        return _id;
    }

    const std::string &Path::get_begin() const
    {
        return _begin;
    }

    const std::string &Path::get_end() const
    {
        return _end;
    }

    long &Path::id()
    {
        return _id;
    }

    std::string &Path::begin()
    {
        return _begin;
    }

    std::string &Path::end()
    {
        return _end;
    }
}