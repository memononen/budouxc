#ifndef BODOUX_H
#define BODOUX_H

// 
// Parse east asian word boundaries based on the BodouX algorithms.
// Each function acceps utf-8 encoded string, and returns results in an array, a flag per input character.
// The flag is set if the codepoint at the input string position starts a new boundary.
//

// Parses Japanese sentence and returns boundary end markers.
void parse_boundaries_ja(const char* text, char* boundaries);

// Parses Simplified Chinese sentence and returns boundary end markers.
void parse_boundaries_zh_hans(const char* text, char* boundaries);

// Parses Traditional Chinese sentence and returns boundary end markers.
void parse_boundaries_zh_hant(const char* text, char* boundaries);

// Parses Thai sentence and returns boundary end markers.
void parse_boundaries_th(const char* text, char* boundaries);

#endif // BODOUX_H