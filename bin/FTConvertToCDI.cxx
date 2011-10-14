//
// This program will convert from an input calibration results file (in the standard text format)
// to a ROOT file for use by the CalibrationDataInterface.
//

#include "Combination/Parser.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace BTagCombination;

void Usage (void);

int main(int argc, char **argv)
{
  //
  // Parse input arguments
  //

  if (argc != 2) {
    Usage();
    return 1;
  }

  string input_file(argv[1]);

  //
  // Parse the input file to extract the calibration
  //

  ifstream input (input_file.c_str());
  if (!input.is_open()) {
    cout << "Unable to open file '" << input_file << "'." << endl;
    return 1;
  }
  vector<CalibrationAnalysis> calib = Parse (input);

  cout << "I found " << calib.size() << " analyses" << endl;

  //
  // Convert it to an output root file.
  //

  return 0;
}

void Usage (void)
{
  cout << "Convert a text file to a CalibrationDataInterface ROOT file" << endl;
  cout << "FTConvertToCDI <input-filename>" << endl;
  cout << "  The output file will have the same name as the input file, with a file" << endl;
  cout << "  extension of .root." << endl;
}
