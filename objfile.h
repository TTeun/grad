#ifndef OBJFILE_H
#define OBJFILE_H

#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector>

class OBJFile {
 public:
  OBJFile(QString fileName);
  OBJFile(QString file, int x);
  ~OBJFile();

  QVector<QVector2D> vertexCoords;
  QVector<QVector3D> vertexColours;
  QVector<unsigned short> faceValences;
  QVector<unsigned int> faceCoordInd;
};

#endif  // OBJFILE_H
