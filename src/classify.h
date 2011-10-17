#ifndef CLASSIFY_H
#define CLASSIFY_H

#include <QList>
#include <QVector>

#include "linear.h"

class Model
{
public:
  class Object: public QVector<feature_node>
  {
    public:
      Object(const QVector<double> &v);
  };

  ~Model();
  static Model *train(QList<QVector<double> > bg, QList<QVector<double> > peds);
  static Model *load(const QString &filename);

  double predict(const Object &obj);

  void save(const QString &filename);
private:
  Model(struct model *_m): m_model(_m) {}
  Model(const Model &) {} // Disabled
  struct model *m_model;
};


#endif // CLASSIFY_H
