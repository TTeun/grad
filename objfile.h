#ifndef OBJFILE_H
#define OBJFILE_H

#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector>

class OBJFile {
 public:
  QVector<QVector2D>      vertexCoords;
  QVector<QVector3D>      vertexColours;
  QVector<unsigned short> faceValences;
  QVector<unsigned int>   faceCoordInd;

 public:
  OBJFile(QString const& fileName);
  OBJFile(QString const& file, int x);
  ~OBJFile();
};

#endif  // OBJFILE_H
