#include "huffman.h"

#include <QDebug>

int main ()
{
  Huffman huffman;

/*****************************************************
  huffman.addElement (2, Huffman::str2ba ("0"));
  huffman.addElement (3, Huffman::str2ba ("10"));
  huffman.addElement (4, Huffman::str2ba ("11"));
*****************************************************/

  huffman.readFromFile ("tree");

  QVector<quint32> decoded = huffman.decode (Huffman::str2ba ("0010101100"));
  qDebug () << decoded;
  qDebug () << "";

  QVector<quint32> decodedWrong = huffman.decode (Huffman::str2ba ("00101011001"));
  qDebug () << decodedWrong;

  return 0;
}
