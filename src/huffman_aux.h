#ifndef HUFFMAN_AUX_H
#define HUFFMAN_AUX_H

#include "huffman_table.h"

namespace Huffman
{

Code str2code (const QString &str, bool *ok = 0);
QString code2str (const Code &code);

int readFromFile (const QString &filename, HuffmanTable<quint32> *huffman);

}

#endif // HUFFMAN_AUX_H
