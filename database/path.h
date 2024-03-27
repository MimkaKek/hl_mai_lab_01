#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Path {
        private:
            long        _id;
            std::string _begin;
            std::string _end;

        public:
            long               get_id() const;
            const std::string &get_begin() const;
            const std::string &get_end() const;

            long&        id();
            std::string &begin();
            std::string &end();

            static void init();
            static std::optional<Path> read_by_id(long id);
            static std::vector<Path> read_all();
            static std::vector<Path> search(std::string begin);
            void save_to_db();

            Poco::JSON::Object::Ptr toJSON() const;
            static Path fromJSON(const std::string& str);

    };
}

#endif