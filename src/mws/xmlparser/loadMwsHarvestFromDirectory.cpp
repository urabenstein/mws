/*

Copyright (C) 2010-2013 KWARC Group <kwarc.info>

This file is part of MathWebSearch.

MathWebSearch is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MathWebSearch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MathWebSearch.  If not, see <http://www.gnu.org/licenses/>.

*/
/**
  * @brief File containing the implementation of the loadMwsHarvestFromDirectory
  * function
  *
  * @file loadMwsHarvestFromDirectory.cpp
  * @date 30 Apr 2012
  *
  * License: GPL v3
  *
  */

// System includes

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <algorithm>
#include <utility>

#include "mws/index/IndexManager.hpp"
#include "common/utils/Path.hpp"
#include "loadMwsHarvestFromFd.hpp"

#include "config.h"

// Namespaces

using namespace std;

namespace mws {

int
loadMwsHarvestFromDirectory(mws::index::IndexManager* indexManager,
                            mws::AbsPath const& dirPath,
                            bool recursive)
{
    DIR*           directory;
    struct dirent* currEntry;
    struct dirent  tmpEntry;
    int            ret;
    size_t         extenSize;
    int            fd;
    int            totalLoaded;
    pair<int,int>  loadReturn;
    vector<string> files;
    vector<string> subdirs;
    AbsPath        fullPath;
    vector<string> :: iterator it;

    totalLoaded = 0;

    extenSize = strlen(MWS_HARVEST_SUFFIX);

    directory = opendir(dirPath.get());
    if (!directory)
    {
        perror("loadMwsHarvestFromDirectory");
        return -1;
    }

    while ((ret = readdir_r(directory, &tmpEntry, &currEntry)) == 0 &&
            currEntry != NULL)
    {
        size_t entrySize = strlen(currEntry->d_name);

        if (currEntry->d_name[0] == '.') {
            printf("Skipping hidden entry \"%s\"\n", currEntry->d_name);
        } else {
            switch (currEntry->d_type) {
            case DT_DIR:
                if (recursive) {
                    subdirs.push_back(currEntry->d_name);
                } else {
                    printf("Skipping directory \"%s\"\n", currEntry->d_name);
                }
                break;
            case DT_REG:
                if (strcmp(currEntry->d_name + entrySize - extenSize,
                             MWS_HARVEST_SUFFIX) == 0) {
                    files.push_back(currEntry->d_name);
                } else {
                    printf("Skipping bad extension file \"%s\"\n",
                           currEntry->d_name);
                }
                break;
            default:
                printf("Skiping entry \"%s\": not a regular file\n",
                       currEntry->d_name);
                break;
            }
        }
    }
    if (ret != 0)
    {
        perror("readdir:");
    }

    // Closing the directory
    closedir(directory);

    // Sorting the entries
    sort(files.begin(), files.end());

    printf("Loading harvest files...\n");

    // Loading the harvests from the respective entries
    for (it = files.begin(); it != files.end(); it++)
    {
        fullPath.set(dirPath.get());
        fullPath.append(*it);
        fd = open(fullPath.get(), O_RDONLY);
        if (fd < 0)
        {
            fprintf(stderr, "Error while opening \"%s\"\n", fullPath.get());
            continue;
        }

        printf("Loading %s... ", fullPath.get());
        fflush(stdout);
        loadReturn = loadMwsHarvestFromFd(indexManager, fd);
        if (loadReturn.first == 0)
        {
            printf("%d loaded\n", loadReturn.second);
        }
        else
        {
            printf("%d loaded (with errors)\n", loadReturn.second);
        }

        totalLoaded += loadReturn.second;

        close(fd);

        printf("Total %d\n", totalLoaded);
    }

    // Recursing through directories
    if (recursive) {
        for (it = subdirs.begin(); it != subdirs.end(); it++) {
            fullPath.set(dirPath.get());
            fullPath.append(*it);
            totalLoaded += loadMwsHarvestFromDirectory(indexManager, fullPath,
                                                       /* recursive = */ true);
        }
    }

    return totalLoaded;
}

}