#include "huffman_table.h"
#include "huffman_aux.h"

#include <QDebug>

int main ()
{
  Huffman::HuffmanTable<quint32> huffman;

  huffman.addElement (2, Huffman::str2code ("0"));
  huffman.addElement (3, Huffman::str2code ("10"));
  huffman.addElement (4, Huffman::str2code ("11"));

/*****************************************************
  readFromFile ("tree", &huffman);
*****************************************************/

  Huffman::Code code = Huffman::str2code ("0010101100");
  QVector<quint32> decoded = huffman.decode (code);
  qDebug () << decoded;
  qDebug () << "";

  Q_ASSERT (huffman.encode (decoded) == code);


  QVector<quint32> decodedWrong = huffman.decode (Huffman::str2code ("00101011001"));
  qDebug () << decodedWrong;

  return 0;
}
