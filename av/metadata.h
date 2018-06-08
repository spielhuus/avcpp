/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef METADATA_H
#define METADATA_H

#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

/** @brief av namespace */
namespace av {

/**
 * @brief The Metadata information.
 *
 * The av::Metadata class provides access to a subset of the metadata information.
 * The provided tags are listen in av::Metadata::Enum.
 *
 * <p>Media tag description and comparison table from http://age.hobba.nl/audio/tag_frame_reference.html</p>
 *
 * @ htmlinclude tag_frame_reference.html
 */
struct Metadata {
public:
    /** @brief enum with metadata elements for read and write access. */
    enum Enum {
        TITLE, ALBUM, ARTIST, COMPOSER, PERFORMER,
        COMMENT, YEAR, TRACK, DISC, GENRE, PUBLISHER
    };

    typedef std::map< std::string, std::string >::iterator iterator;

    Metadata() {}
    ~Metadata() {}

    std::string tagname ( const Enum tag );

    /** @brief set metadata with value by key. */
    void set (
        /** The metadata key */ const std::string& name,
        /** The value for the metadata */ const std::string& value
    );
    /** @brief get the metadata key type as string.
        @return the ffmpeg key as string. */
    static std::string name ( /** The key to get as string. */ const Enum& tag );
    /** @brief get the metadata value by key.
        @return the value referenced by key. */
    std::string& get ( /** The key for the metadata element. */ const std::string& key );
    /** @brief get keys of valid metadata elments.
        @return the keys of the defined metadata elements */
    std::vector< std::string > tag_names ();

    friend std::ostream& operator<< ( std::ostream& stream, const Metadata& meta_data ) {
        for ( auto& __tag : meta_data.tags ) {
            stream << __tag.first << ": \t\t" << __tag.second << "\n";
        }

        return stream;
    }
    size_t size() { return tags.size(); }

    iterator begin()
    {return tags.begin();}
    iterator end()
    {return tags.end();}

private:

    const static std::string _TITLE;
    const static std::string _ALBUM;
    const static std::string _ARTIST;
    const static std::string _COMPOSER;
    const static std::string _PERFORMER;
    const static std::string _COMMENT;
    const static std::string _YEAR;
    const static std::string _TRACK;
    const static std::string _DISC;
    const static std::string _GENRE;
    const static std::string _PUBLISHER;

    const static std::array< std::string, 11 > _tag_names;
    std::map< std::string, std::string > tags;
    static const std::array< std::string, 10 > tag_names_;
};
}//namespace av
#endif // METADATA_H
