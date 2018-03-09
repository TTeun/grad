#include "objfile.h"

#include <QDebug>
#include <QFile>

OBJFile::OBJFile(QString fileName) {
  qDebug() << "✓✓ OBJFile constructor";

  qDebug() << ":: Loading" << fileName;
  QFile newModel(fileName);

  if (newModel.open(QIODevice::ReadOnly)) {
    QTextStream fileContents(&newModel);

    QString currentLine;
    QStringList values;
    QStringList indices;

    unsigned short k;

    vertexCoords.clear();
    vertexColours.clear();
    faceCoordInd.clear();

    while (!fileContents.atEnd()) {
      currentLine = fileContents.readLine();
      values = currentLine.split(" ");

      if (values[0] == "v") {
        vertexCoords.append(
            QVector2D(values[1].toFloat(), values[2].toFloat()));
        if (values.size() != 3 && values.size() != 4 && values.size() != 5)
          vertexColours.append(QVector3D(
              values[3].toFloat(), values[4].toFloat(), values[5].toFloat()));
        else
          vertexColours.append(QVector3D(0.2, 0.4, 0.6));
      }

      else if (values[0] == "f") {
        for (k = 1; k < values.size(); k++) {
          indices = values[k].split("/");

          // Note -1, OBJ starts indexing from 1.
          faceCoordInd.append(indices[0].toInt() - 1);
        }

        faceValences.append(k - 1);

      } else {
        qDebug() << " * Line contents ignored," << currentLine;
      }
    }

    newModel.close();
  }
  qDebug() << "OBJFile constructor complete";
}

OBJFile::~OBJFile() {
  qDebug() << "✗✗ OBJFile destructor";
}
