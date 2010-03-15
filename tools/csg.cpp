/*
  This file is part of SolidarityCSG.

  SolidarityCSG is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  SolidarityCSG is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with SolidarityCSG.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <SolidarityCSG.h>
#include <tinyxml.h>

using namespace csg;
using namespace std;



/// Main application entry
int main(int argc, char ** argv)
{
  // Check/get arguments
  if(argc != 2)
  {
    cout << "Usage: " << argv[0] << " job-xml" << endl;
    return -1;
  }
  char * inFile = argv[1];

  try
  {
    // Load input XML
    cout << "Loading job description " << inFile << "..." << endl;
    // FIXME
  }
  catch(exception &e)
  {
    cout << "Error: " << e.what() << endl;
  }

  return 0;
}