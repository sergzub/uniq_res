#include "fd_holder.h"

#include <iostream>

#include <fcntl.h>


int main(int argc, char* argv[])
{
    FdHolder fd(::open(argv[0], O_RDONLY) );
    if( fd )
    {
        std::cout << "I opened myself: '" << argv[0] << "' (fd=" << fd << ")" << std::endl;
    }
    else
    {
        std::cerr << "Failed to open myself: '" << argv[0] << '\'' << std::endl;
    }
    
    return 0;
}
