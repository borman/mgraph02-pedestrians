#ifndef CLASSIFY_H
#define CLASSIFY_H

#include <QList>
#include <QVector>

class Model
{
public:
  ~Model();
  static Model *train(QList<QVector<double> > bg, QList<QVector<double> > peds);

  void save(const QString &filename);
private:
  Model(struct model *_m): m_model(_m) {}
  Model(const Model &) {} // Disabled
  struct model *m_model;
};


#endif // CLASSIFY_H
