#ifndef SPRITEVIEW_H
#define SPRITEVIEW_H

#include "AssetView.h"

#include <QObject>
#include <QWidget>

class SpriteView : public AssetView {
  Q_OBJECT

  Q_PROPERTY(QPixmap Image MEMBER _pixmap NOTIFY ImageChanged USER true)

 public:
  SpriteView(AssetScrollAreaBackground *parent);
  QSize sizeHint() const override;
  void Paint(QPainter &painter) override;
  void PaintTop(QPainter &painter) override;
  QRectF AutomaticBBoxRect();
  QPixmap &GetPixmap();
  QRectF BBoxRect();

 signals:
  void ImageChanged(const QPixmap &newImage);

 public slots:
  void SetSubimage(int index);
  void SetShowBBox(bool show);
  void SetShowOrigin(bool show);

 private:
  QPixmap _pixmap;
  bool _showBBox;
  bool _showOrigin;
};

#endif  // SPRITEVIEW_H
