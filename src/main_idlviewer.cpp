#include <QApplication>
#include <QAbstractListModel>
#include <QDir>
#include <QDebug>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeImageProvider>

#include "idl.h"
#include "gradient.h"
#include "colorcorrect.h"

class IDLModel: public QAbstractListModel
{
  public:
    enum Role
    {
      X = Qt::UserRole + 1,
      Y = Qt::UserRole + 2,
      Height = Qt::UserRole + 3,
      Width = Qt::UserRole + 4,
      FileName = Qt::UserRole + 5
    };

    IDLModel(const QList<IDL::Entry> &_d)
      : m_data(_d)
    {
      QHash<int, QByteArray> roles;
      roles.insert(X, "rx");
      roles.insert(Y, "ry");
      roles.insert(Width, "rwidth");
      roles.insert(Height, "rheight");
      roles.insert(FileName, "filename");
      setRoleNames(roles);
    }

    QVariant data(const QModelIndex &index, int role) const
    {
      int i = index.row();
      switch (role)
      {
      case X: return m_data[i].rects[0].x();
      case Y: return m_data[i].rects[0].y();
      case Width: return m_data[i].rects[0].width();
      case Height: return m_data[i].rects[0].height();
      case FileName: return m_data[i].name;
      }
      return QVariant();
    }

    int rowCount(const QModelIndex &parent) const
    {
      return m_data.size();
    }
  private:
    QList<IDL::Entry> m_data;
};

class IDLImageProviderSrc: public QDeclarativeImageProvider
{
  public:
    IDLImageProviderSrc(const QDir &dir)
      : QDeclarativeImageProvider(QDeclarativeImageProvider::Image),
        m_dir(dir)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
      Q_UNUSED(requestedSize);
      QString filename = m_dir.filePath(id + ".png");
      QImage img(filename);
      *size = img.size();
      return img;
    }
  private:
    QDir m_dir;
};

class IDLImageProviderGrad: public QDeclarativeImageProvider
{
  public:
    IDLImageProviderGrad(const QDir &dir)
      : QDeclarativeImageProvider(QDeclarativeImageProvider::Image),
        m_dir(dir)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
      Q_UNUSED(requestedSize);
      QString filename = m_dir.filePath(id + ".png");
      QImage img(filename);
      QRect rect(0, 0, img.width(), img.height());
      img = showGradient(gradient(img, rect));
      rgb_stretch(img, rect);
      *size = img.size();
      return img;
    }
  private:
    QDir m_dir;
};


int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  if (argc < 2)
  {
    qWarning() << "No arguments supplied";
    return 1;
  }

  try
  {
    QDir srcDir = QFileInfo(argv[1]).dir();
    QList<IDL::Entry> objects = IDL::load(argv[1]);
    IDLModel objectsModel(objects);

    QDeclarativeView view;
    view.rootContext()->setContextProperty("files", &objectsModel);
    view.engine()->addImageProvider(QLatin1String("src"), new IDLImageProviderSrc(srcDir));
    view.engine()->addImageProvider(QLatin1String("grad"), new IDLImageProviderGrad(srcDir));
    view.setSource(QUrl("qrc:///idlviewer.qml"));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.resize(640, 480);
    view.show();

    return app.exec();
  }
  catch (const QString &e)
  {
    qWarning() << "Fatal exception: " << e;

    return 1;
  }
}
