#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <QVector>
#include <QBitArray>

class QString;

/**
 * Class representing huffman table.
 * Supports only decoding of data.
 * Typical usage - read () then decode ().
 */

class Huffman
{
  public:
    Huffman ();
    ~Huffman ();

    int addElement (const quint32 element, const QBitArray &code);

    int readFromFile (const QString &filename);

    QVector<quint32> decode (const QBitArray &data, bool *ok = NULL) const;

    void clear ();

    static QBitArray str2ba (const QString &str, bool *ok = NULL);
    static QString ba2str (const QBitArray &ba);

  private:
    struct TreeNode
    {
      TreeNode () : left (NULL), right (NULL), element (0) {}

      bool isLeaf () const { return left == NULL && right == NULL; }

      TreeNode *left, *right;
      quint32 element;

      Q_DISABLE_COPY (TreeNode);
    };

    static void deleteTree (TreeNode *node);

    TreeNode *root_;

    Q_DISABLE_COPY (Huffman);
};

#endif // HUFFMAN_H
