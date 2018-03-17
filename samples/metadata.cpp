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
#include <iostream>

#include "../av/format.h"

int main ( int argc, char* argv[] ) {

    if ( argc <= 1 ) {
        fprintf ( stderr, "Usage: %s <input file>\n", argv[0] );
        exit ( 0 );
    }

    std::cout << "Open File: " << argv[1] << "\n";
    av::Format format ( argv[1] );

    if ( !!format ) {
        std::cout << "Error: " << format.errc().message() << std::endl;
        exit ( 1 );
    }

    for ( auto codec : format ) {
        std::cout << ( *codec ) << "\n";
    }

    auto metadata_ = format.metadata();
    std::cout << metadata_ <<  std::endl;
    return 0;
}
