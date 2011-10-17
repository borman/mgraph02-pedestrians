#include <QString>
#include <QPainter>

#include "linear.h"

#include "hog.h"
#include "classify.h"


Model *Model::train(QList<QVector<double> > bg, QList<QVector<double> > peds)
{
  Q_ASSERT(bg.size() != 0 && peds.size() != 0);

  struct problem prob;
  prob.l = bg.size() + peds.size();
  prob.bias = -1;
  prob.n = bg[0].size();

  prob.y = new int[prob.l]; // labels
  prob.x = new feature_node *[prob.l]; // features

  for (int i=0; i<prob.l; i++)
  {
    prob.x[i] = new feature_node[prob.n+1];
    prob.x[i][prob.n].index = -1;
    for (int j=0; j<prob.n; j++)
    {
      prob.x[i][j].index = j+1;
      prob.x[i][j].value = (i < bg.size())? bg[i][j] : peds[i-bg.size()][j];
    }
    prob.y[i] = (i < bg.size())? 1 : -1;
  }

  struct parameter param;
  param.solver_type = L2R_L2LOSS_SVC_DUAL;
  param.C = 1;
  param.eps = 1e-4;
  param.nr_weight = 0;
  param.weight_label = NULL;
  param.weight = NULL;

  struct model *mdl = ::train(&prob, &param);

  ::destroy_param(&param);
  for (int i=0; i<prob.l; i++)
    delete[] prob.x[i];
  delete[] prob.y;
  delete[] prob.x;

  return new Model(mdl);
}

Model::~Model()
{
  ::free_and_destroy_model(&m_model);
}

void Model::save(const QString &filename)
{
  ::save_model(filename.toUtf8().constData(), m_model);
}

Model *Model::load(const QString &filename)
{
  struct model *mdl = load_model(filename.toUtf8().constData());
  if (mdl)
    return new Model(mdl);
  else
    return 0;
}

double Model::predict(const Model::Object &obj)
{
  double est[2];
  ::predict_values(m_model, obj.constData(), est);
  return -est[0];
}

Model::Object::Object(const QVector<double> &v)
  : QVector<feature_node>(v.size()+1)
{
  for (int i=0; i<v.size(); i++)
  {
    (*this)[i].index = 1+i;
    (*this)[i].value = v[i];
  }
  last().index = -1;
}

QImage Model::probMap(const QImage &img)
{
  QImage map(img.width(), img.height(), QImage::Format_ARGB32);
  map.fill(qRgba(0, 0, 0, 0));

  HOG hog(gradient(img, QRect(0, 0, img.width(), img.height())));
  hog.normalize();

  QVector<double> ps(hog.width()-9);
  double upper = 0.1;
  double lower = -0.1;
  for (int x=0; x<ps.size(); x++)
  {
    ps[x] = predict(hog.serialize(x, 0, 10, hog.height()));
    upper = qMax(upper, ps[x]);
    lower = qMin(lower, ps[x]);
  }

  QPainter p;
  p.begin(&map);
  for (int x=0; x<ps.size()-9; x++)
    p.fillRect(x*8, 0, 80, map.height(),
               QColor(qMin(0.0, ps[x])*255.0/lower,
                      qMax(0.0, ps[x])*255.0/upper,
                      0,
                      ps[x]>0? 100 : 40));
  p.end();

  return map;
}
