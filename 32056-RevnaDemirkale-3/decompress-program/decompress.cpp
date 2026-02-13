#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/*
 * Constants
 * ---------
 * MAX_CODES  : Maximum number of dictionary entries (2^12 for LZW)
 * FIRST_CODE : First available code after ASCII characters
 */
const int MAX_CODES = 4096;
const int FIRST_CODE = 256;

int main() {
    /*
     * Input and output files
     * ----------------------
     * "compout"   : compressed input file (text format, codes separated by spaces)
     * "decompout" : decompressed output file (binary mode)
     */
    ifstream in("compout");
    ofstream out("decompout", ios::binary);

    // Check if input file opened successfully
    if (!in) {
        cerr << "Cannot open compout.\n";
        return 1;
    }

    // Check if output file opened successfully
    if (!out) {
        cerr << "Cannot open decompout.\n";
        return 1;
    }

    /*
     * Dictionary
     * ----------
     * Stores strings indexed by their LZW codes.
     * Size is fixed to MAX_CODES.
     */
    vector<string> dict(MAX_CODES);

    /*
     * Initialize dictionary with single-character ASCII strings
     * Codes 0–255 correspond to standard ASCII characters.
     */
    for (int i = 0; i < 256; ++i) {
        dict[i] = string(1, static_cast<char>(i));
    }

    // Next available dictionary code
    int nextCode = FIRST_CODE;

    /*
     * Read the first code
     * -------------------
     * This initializes the decompression process.
     */
    int prevCode;
    if (!(in >> prevCode)) {
        // Empty input file
        return 0;
    }

    // Output the string corresponding to the first code
    string prevStr = dict[prevCode];
    out << prevStr;

    /*
     * Main decompression loop
     * -----------------------
     * Reads codes one by one and reconstructs the original data.
     */
    int currCode;
    while (in >> currCode) {
        string entry;

        /*
         * If the current code already exists in the dictionary,
         * use the corresponding string.
         */
        if (!dict[currCode].empty()) {
            entry = dict[currCode];
        }
        /*
         * Special LZW case:
         * If the code is not yet in the dictionary,
         * the entry is previous string + its first character.
         */
        else {
            entry = prevStr + prevStr[0];
        }

        // Write the decoded string to the output file
        out << entry;

        /*
         * Add a new entry to the dictionary:
         * previous string + first character of current entry
         */
        if (nextCode < MAX_CODES) {
            string newStr = prevStr + entry[0];
            dict[nextCode] = newStr;
            nextCode++;
        }

        // Update previous string for the next iteration
        prevStr = entry;
    }

    return 0;
}
