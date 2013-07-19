#ifndef HUFFMAN_TABLE_H
#define HUFFMAN_TABLE_H

#include <QVector>
#include <QDebug>
#include <QStack>

/**
 * Class representing huffman table.
 * Supports only decoding of data.
 * Typical usage - read () then decode ().
 */

namespace Huffman
{

typedef QVector<bool> Code;

template <typename T>
class HuffmanTable
{
  public:
    HuffmanTable ();
    ~HuffmanTable ();

    int addElement (const T &element, const Code &code);

    Code encode (const QVector<T> &message, bool *ok = NULL) const;
    QVector<T> decode (const Code &data, bool *ok = NULL) const;

    Code encodeBest (const QVector<T> &message);

    void clear ();

  private:
    typedef QHash<T, Code> EncodingTable;
    struct TreeNode
    {
      TreeNode () : left (NULL), right (NULL), element () {}

      bool isLeaf () const { return left == NULL && right == NULL; }

      TreeNode *left, *right;
      T element;

      Q_DISABLE_COPY (TreeNode);
    };

    Code findCode (const T &element) const;

    static void deleteTree (TreeNode *node);

    TreeNode *root_;

    EncodingTable encTable_;

    Q_DISABLE_COPY (HuffmanTable);
};

template <typename T>
HuffmanTable<T>::HuffmanTable ()
  : root_ (NULL)
{
}

template <typename T>
HuffmanTable<T>::~HuffmanTable ()
{
  this->clear ();
}

template <typename T>
int HuffmanTable<T>::addElement (const T &element, const Code &code)
{
  /// Adds element to the huffman table.
  /// Returns 0 on success, < 0 if error.

  try
  {
    if (!root_)
    {
      root_ = new TreeNode;
    }

    if (encTable_.contains (element))
    {
      qDebug () << "Repeated code for element. Skipping.";
      return -1;
    }

    encTable_[element] = code;

    TreeNode *cur = root_;
    int position = 0;

    while (position < code.size ())
    {
      if (code[position])
      {
        if (!cur->right)
        {
          cur->right = new TreeNode;
        }
        cur = cur->right;
      }
      else
      {
        if (!cur->left)
        {
          cur->left = new TreeNode;
        }
        cur = cur->left;
      }

      ++position;
    }

    // We assert that the node we arrived at is a leaf node.
    // Otherwise - error.
    if (!cur->isLeaf ())
    {
      qDebug () << QObject::tr ("Error: Inconsistent huffman table");
      return -2;
    }

    // FIXME: we could add another variable to TreeNode to check whether
    // an element was set to the node. This would allow us to detect different
    // elements assigned to a single nodes and elements assigned to non-leaf
    // nodes before they became non-leaf.

    cur->element = element;
  }
  catch (std::bad_alloc)
  {
    qDebug () << QObject::tr ("Not enough memory");
    this->clear ();
    return -2;
  }

  return 0;
}

template <typename T>
Code HuffmanTable<T>::encode (const QVector<T> &message, bool *ok) const
{
  /// Encodes message according to previously loaded table.
  /// If error - sets ok to false and returns message encoded so far.

  Code res;

  if (ok)
    *ok = false;

  for (typename QVector<T>::const_iterator it = message.begin ();
      it != message.end (); ++it)
  {
    Code code = this->findCode (*it);

    if (!code.size ())
    {
      qDebug () << "Error: cannot encode message: symbol is not in table.";
      if (ok)
        *ok = 0;
      return res;
    }

    res += code;
  }

  return res;
}

template <typename T>
QVector<T> HuffmanTable<T>::decode (const Code &data, bool *ok) const
{
  /// Decodes data according to loaded huffman table.
  /// If error - sets ok to false and returns decoded array constructed so far.

  if (ok)
    *ok = true;

  QVector<T> res;
  if (root_ == NULL)
  {
    qDebug () << QObject::tr ("Error: Table is not initialized.");
    if (ok)
      *ok = false;

    return res;
  }

  TreeNode *cur = root_;

  int i = 0;
  while (i < data.size ())
  {
    if (cur == NULL)
    {
      break;
    }

    if (cur->isLeaf ())
    {
      res.push_back (cur->element);
      cur = root_;
    }
    else
    {
      cur = data[i] ? cur->right : cur->left;
      ++i;
    }
  }
  
  // If we are not in a leaf node - data was not valid
  if (cur == NULL || !cur->isLeaf ())
  {
    qDebug () << QObject::tr ("Data to decode doesn't match huffman table.");
    if (ok)
      *ok = false;
  }
  else
  {
    res.push_back (cur->element);
  }

  return res;
}

template <typename T>
void HuffmanTable<T>::clear ()
{
  if (root_)
  {
    deleteTree (root_);
  }

  root_ = NULL;
}

template <typename T>
Code HuffmanTable<T>::findCode (const T &element) const
{
  typename EncodingTable::const_iterator it = encTable_.find (element);

  if (it == encTable_.end ())
  {
    return Code ();
  }

  return it.value ();
}

template <typename T>
void HuffmanTable<T>::deleteTree (TreeNode *node)
{
  // Realization was switched from recursive to stack based to handle
  // the (unlikely) case of very large tree.
  if (node == NULL)
    return;

  QStack<TreeNode *> stack;

  stack.push (node);

  while (!stack.empty ())
  {
    TreeNode *cur = stack.pop ();
    if (cur->left)
      stack.push (cur->left);
    if (cur->right)
      stack.push (cur->right);
    delete cur;
  }

/*****************************************************
  deleteTree (node->left);
  deleteTree (node->right);
*****************************************************/
}

}

#endif // HUFFMAN_H
