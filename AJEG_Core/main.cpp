//
//  main.cpp
//  AJEG_Core
//
//  Created by Sebastian Mach on 14.01.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#include <iostream>
#include "AJEG_Core.h"

static void printUsage(char *programPath);



int main(int argc, const char * argv[])
{
    std::cout << "AJEG Core -- By Sebastian Mach"                          << std::endl;
    std::cout << "Aggressive JPEG Engineering Group (AJEG)"                << std::endl;
    std::cout << "Copyright (c) 2015 Sebastian Mach. All rights reserved." << std::endl << std::endl;
    
    
    std::string inputFile  = "./input.pbm";
    std::string outputFile = "./output.jpg";
    
    
    int arg_index = 1;
    while ((arg_index < argc) && (argv[arg_index][0] == '-'))
    {
        switch (tolower(argv[arg_index][1]))
        {
            case 'i':
                inputFile = argv[arg_index + 1];
                arg_index ++;
                break;
            case 'o':
                outputFile = argv[arg_index + 1];
                arg_index ++;
                break;
            case 'h':
                printUsage((char*)argv[0]);
                break;
            default:
                std::cout << AC_ERROR << "Unrecognized argument: " << argv[arg_index] << std::endl;
                return EXIT_FAILURE;
        }
        arg_index++;
    }
    
    
    AJEG_Core core(inputFile, outputFile);
    aj_image *image = core.loadRGBImage();
    return core.jo_encode(image);
    
    
    return EXIT_SUCCESS;
}






static void printUsage(char *programPath) {
    std::cout << "Usage: " << programPath << " <options>" << std::endl
              << "Options:"                               << std::endl
              << "\t-h,\t Show this message"              << std::endl
              << "\t-i,\t Path to the input file"         << std::endl
              << "\t-o,\t Path to the output file"        << std::endl
              << "\t-p,\t Print information about the input file to the output" << std::endl
              << std::endl;
}



