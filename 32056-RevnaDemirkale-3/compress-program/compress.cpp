#include <iostream>
#include <fstream>
#include <string>
#include "HashTable.h"

using namespace std;

/*
 * Constants
 * ---------
 * MAX_CODES  : Maximum number of dictionary entries (4096 = 2^12)
 * FIRST_CODE : First available code after ASCII characters
 */
const int MAX_CODES = 4096;
const int FIRST_CODE = 256;

int main() {
    /*
     * Input and output files
     * ----------------------
     * "compin"  : original input file (binary mode)
     * "compout" : compressed output file (text, codes separated by spaces)
     */
    ifstream in("compin", ios::binary);
    ofstream out("compout");

    // Check if input file opened successfully
    if (!in) {
        cerr << "Cannot open compin.\n";
        return 1;
    }

    // Check if output file opened successfully
    if (!out) {
        cerr << "Cannot open compout.\n";
        return 1;
    }

    /*
     * Dictionary
     * ----------
     * HashTable mapping strings to integer codes.
     * Used for fast lookup during compression.
     */
    HashTable<string, int> dict;

    /*
     * Initialize dictionary with single-character ASCII strings
     * Codes 0–255 represent standard ASCII characters.
     */
    for (int i = 0; i < 256; ++i) {
        string s(1, static_cast<char>(i));
        dict.insert(s, i);
    }

    // Next available dictionary code
    int nextCode = FIRST_CODE;

    char c;
    string p;

    /*
     * Read the first character
     * ------------------------
     * If the file is empty, exit.
     */
    if (!in.get(c)) {
        return 0;
    }

    // Initialize p with the first character
    p = string(1, c);

    // Used to format output (no leading space for first code)
    bool firstOutput = true;

    /*
     * Main compression loop (LZW)
     * ---------------------------
     * Reads characters one by one and builds dictionary entries.
     */
    while (in.get(c)) {
        string pc = p + c;
        int dummy;

        /*
         * If p+c exists in the dictionary,
         * extend the current string.
         */
        if (dict.find(pc, dummy)) {
            p = pc;
        }
        /*
         * Otherwise:
         * 1. Output the code for p
         * 2. Add p+c to the dictionary
         * 3. Reset p to the current character
         */
        else {
            int codeP;
            dict.find(p, codeP);

            // Output the code with correct spacing
            if (firstOutput) {
                out << codeP;
                firstOutput = false;
            } else {
                out << " " << codeP;
            }

            // Add new entry to the dictionary if space is available
            if (nextCode < MAX_CODES) {
                dict.insert(pc, nextCode);
                nextCode++;
            }

            // Reset p to the current character
            p = string(1, c);
        }
    }

    /*
     * Output the code for the last string p
     */
    if (!p.empty()) {
        int codeP;
        dict.find(p, codeP);

        if (firstOutput)
            out << codeP;
        else
            out << " " << codeP;
    }

    // Trailing space (used by the decompressor)
    out << " ";

    return 0;
}
